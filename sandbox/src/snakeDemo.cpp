#include <iostream>
#include <cmath>
#include <random>

#include <core/EntryPoint.hpp>
#include <core/EngineApp.hpp>

#include <utilities/Logger.hpp>
#include <events/types/WindowCloseEvent.hpp>
#include <layers/Layer.hpp>
#include <ecs/types/EngineComponents.hpp>
#include <core/ResourceRegistry.hpp>
#include "ecs/types/BaseComponent.hpp"

using namespace TileBite;

struct VelocityComponent : public BaseComponent {
    float v = 0.0f;
    float startingV = 0.0f;

    VelocityComponent(float velocity = 0.0f, float startV = 0.0f)
        : v(velocity), startingV(startV) {
    }
};

struct Leader : public BaseComponent {
    ID target = 0;
    bool hasTarget = false;
    ID lastBodyPart = 0;
    int bodyPartsCount = 0;
    glm::vec4 color = glm::vec4(1.0f);

    Leader(ID t = 0, bool h = false, ID l = 0, int b = 0, glm::vec4 c = glm::vec4(1.0f))
        : target(t), hasTarget(h), lastBodyPart(l), bodyPartsCount(b), color(c) {
    }
};

struct Follower : public BaseComponent {
    ID target = 0;

    Follower(ID t = 0)
        : target(t) {
    }
};

struct Food : public BaseComponent {
    bool isTargeted = false;

    Food(bool targeted = false)
        : isTargeted(targeted) {
    }
};

class LeaderSystem : public ISystem {
public:

    void update(float deltaTime) override
    {
        auto& world = EngineApp::getInstance()->getSceneManager().getActiveScene()->getWorld();

        world.query<TransformComponent, VelocityComponent, Leader>().each(
            [&](ID id, TransformComponent* t, VelocityComponent* v, Leader* l) {
                if (l->hasTarget) {
                    if (!world.entityExists(l->target))
                    {
                        l->hasTarget = false;
                        return;
                    }

                    TransformComponent* foodTransform = world.getComponent<TransformComponent>(l->target);

                    float dx = foodTransform->getPosition().x - t->getPosition().x;
                    float dy = foodTransform->getPosition().y - t->getPosition().y;
                    float distance = std::sqrt(dx * dx + dy * dy);
                    if (distance > foodTransform->getSize().x) {
                        glm::vec2 newOffset = glm::vec2(
                            (dx / distance) * v->v * deltaTime,
                            (dy / distance) * v->v* deltaTime
                        );
                        t->setPosition(t->getPosition() + newOffset);
                        t->setRotation(std::atan2(dy, dx));
                    }
                    else
                    {
                        l->hasTarget = false;
                        world.removeEntity(l->target);

                        ID lastBodyPart = id;
                        if (l->bodyPartsCount > 0)
                            lastBodyPart = l->lastBodyPart;
                        l->bodyPartsCount++;

                        // Create new body part
                        glm::vec4 color = l->color;

                        ID followerEntity = world.createEntity();
                        l->lastBodyPart = followerEntity;
                        world.addComponents(
                            followerEntity,
                            TransformComponent{ glm::vec2(foodTransform->getPosition().x, foodTransform->getPosition().y), glm::vec2(0.01, 0.01) },
                            SpriteComponent{ color, 0 },
                            VelocityComponent{ 2, 2 },
                            Follower{ lastBodyPart }
                        );

                        v->v = v->startingV / (1.0f + l->bodyPartsCount * 0.01f);
                    }
                }
                else
                {
                    // Find nearest food
                    float closestDistance = std::numeric_limits<float>::max();
                    ID closestFoodID = 0;
                    bool found = false;
                    world.query<TransformComponent, Food>().each([&](ID entityID, TransformComponent* foodTransform, Food* f) {
                        if (f->isTargeted) return; // Skip if food is already targeted

                        float dx = foodTransform->getPosition().x - t->getPosition().x;
                        float dy = foodTransform->getPosition().y - t->getPosition().y;
                        float distance = std::sqrt(dx * dx + dy * dy);
                        if (distance < closestDistance) {
                            closestDistance = distance;
                            closestFoodID = entityID;
                            found = true;
                        }
                        });

                    if (found)
                    {
                        l->target = closestFoodID;
                        l->hasTarget = true;
                        Food* foodComp = world.getComponent<Food>(l->target);
                        foodComp->isTargeted = true;
                    }
                }
            }
        );
    }
};

class FollowerSystem : public ISystem {
public:
    void update(float deltaTime) override
    {
        auto& world = EngineApp::getInstance()->getSceneManager().getActiveScene()->getWorld();

        std::unordered_map<ID, TransformComponent*> transformCache;
        world.query<TransformComponent>().each([&](ID id, TransformComponent* t) {
            transformCache[id] = t;
            });

        world.query<TransformComponent, VelocityComponent, Follower>().each(
            [&](ID id, TransformComponent* t, VelocityComponent* v, Follower* f) {
                auto it = transformCache.find(f->target);

                TransformComponent* target = it->second;
                float dx = target->getPosition().x - t->getPosition().x;
                float dy = target->getPosition().y - t->getPosition().y;
                float distance = std::sqrt(dx * dx + dy * dy);
                if (distance > target->getSize().x + 0.01f) {
                    glm::vec2 newOffset = glm::vec2(
                        (dx / distance) * v->v * deltaTime,
                        (dy / distance) * v->v * deltaTime
                    );
                    t->setPosition(t->getPosition() + newOffset);
                    t->setRotation(std::atan2(dy, dx));
                }
            }
        );
    }

};

class FoodSystem : public ISystem {
public:
    void update(float deltaTime) override
    {
        auto& world = EngineApp::getInstance()->getSceneManager().getActiveScene()->getWorld();

        world.query<TransformComponent, VelocityComponent, Food>().each(
            [&](ID id, TransformComponent* t, VelocityComponent* v, Food* f) {
                glm::vec2 newOffset = glm::vec2(std::cos(t->getRotation()), std::sin(t->getRotation())) * v->v * deltaTime;
                t->setPosition(t->getPosition() + newOffset);

                // Bounce off X edges
                if (t->getPosition().x < -1.0f || t->getPosition().x > 1.0f) {
                    t->setRotation(glm::pi<float>() - t->getRotation());
                    t->setPosition(t->getPosition() + glm::vec2(glm::clamp(t->getPosition().x, -1.0f, 1.0f), 0));
                }

                // Bounce off Y edges
                if (t->getPosition().y < -1.0f || t->getPosition().y > 1.0f) {
                    t->setRotation(-t->getRotation());
                    t->setPosition(t->getPosition() + glm::vec2(0, glm::clamp(t->getPosition().y, -1.0f, 1.0f)));
                }
            }
        );
    }
};

class MainScene : public Scene {
    void createSnake(float speed)
    {
        int followerCount = 10;
        float size = 0.015;

        float rx = quickRandFloat(-1.0f, 1.0f);
        float ry = quickRandFloat(-1.0f, 1.0f);

        glm::vec4 color = glm::vec4(
            quickRandFloat(0.4f, 1.0f),            // R bright
            quickRandFloat(0.0f, 0.3f),            // G low to avoid green tint
            quickRandFloat(0.4f, 1.0f),            // B bright
            1.0f
        );

        // create leader
        ID leaderEntity = getWorld().createEntity();
        getWorld().addComponents(
            leaderEntity,
            TransformComponent{ glm::vec2(rx, ry), glm::vec2(size, size) },
            SpriteComponent{ glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 0 },
            VelocityComponent{ speed, speed },
            Leader{ 0, false, 0, 0, color }
        );
    }

    void onLoad() override
    {
        auto cameraController = std::make_shared<CameraController>();

        getSystemManager().addSystem(std::make_unique<FollowerSystem>());
        getSystemManager().addSystem(std::make_unique<LeaderSystem>());
        getSystemManager().addSystem(std::make_unique<FoodSystem>());
        setCameraController(cameraController);

        int foodC = 2000;
        int snakeC = 15;

        // create food
        for (int i = 0; i < foodC; i++)
        {
            float randx = quickRandFloat(-1.0f, 1.0f);
            float randy = quickRandFloat(-1.0f, 1.0f);
            float r = quickRandFloat(0.01f, 0.05f);
            float foodSpeed = quickRandFloat(-0.04, 0.04);
            float rSize = quickRandFloat(0.003f, 0.01f);
            ID foodEntity = getWorld().createEntity();
            getWorld().addComponents(
                foodEntity,
                TransformComponent{ glm::vec2(randx, randy), glm::vec2(rSize, rSize), r },
                VelocityComponent{ foodSpeed, foodSpeed },
                SpriteComponent{ glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 0 },
                Food{ false }
            );
        }

        for (size_t i = 0; i < snakeC; i++)
        {
            float rS = quickRandFloat(0.1, 0.4);
            createSnake(rS);
        }
    }
};

class GameLayer : public Layer {
public:

    void onAttach() override
    {
        auto scene = EngineApp::getInstance()->getSceneManager().createScene<MainScene>("MainScene");
        EngineApp::getInstance()->getSceneManager().setActiveScene(scene);
    }
};

class MyApp : public TileBite::EngineApp {
    AppConfig config() override { return AppConfig(800, 600, "Snake demo"); }

    void setup() override
    {
        pushLayer(std::make_unique<GameLayer>());
    }
};

CREATE_ENGINE_APPLICATION(MyApp)