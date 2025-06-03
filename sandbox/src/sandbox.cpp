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

using namespace Engine;

inline float quickRandFloat(float min = -1.0f, float max = 1.0f) {
    return min + (max - min) * (rand() / float(RAND_MAX));
}

struct VelocityComponent {
    float vx, vy;
};

struct Leader {
    ID target;
	bool hasTarget = false;
};

struct Follower {
    ID target;
};

struct Food {
    bool isTargeted;
};

class LeaderSystem : public ISystem {
public:

    void update(float deltaTime) override
    {
        auto& world = getSceneManager()->getActiveScene()->getWorld();

        world.query<TransformComponent, VelocityComponent, Leader>().each(
            [&](ID id, TransformComponent* t, VelocityComponent* v, Leader* l) {
				if (l->hasTarget) {
                    if (!world.entityExists(l->target))
                    {
						l->hasTarget = false;
                        return;
                    }

                    TransformComponent* foodTransform = world.getComponent<TransformComponent>(l->target);
					
                    float dx = foodTransform->Position.x - t->Position.x;
					float dy = foodTransform->Position.y - t->Position.y;
					float distance = std::sqrt(dx * dx + dy * dy);
					if (distance > foodTransform->Size.x + 0.01f) {
						t->Position.x += (dx / distance) * v->vx * deltaTime;
						t->Position.y += (dy / distance) * v->vy * deltaTime;
						t->Rotation = std::atan2(dy, dx);
					}
                    else
                    {
                        l->hasTarget = false;
						world.removeEntity(l->target);
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

						float dx = foodTransform->Position.x - t->Position.x;
						float dy = foodTransform->Position.y - t->Position.y;
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
        auto& world = getSceneManager()->getActiveScene()->getWorld();

        std::unordered_map<ID, TransformComponent*> transformCache;
        world.query<TransformComponent>().each([&](ID id, TransformComponent* t) {
            transformCache[id] = t;
        });

        world.query<TransformComponent, VelocityComponent, Follower>().each(
            [&](ID id, TransformComponent* t, VelocityComponent* v, Follower* f) {
                auto it = transformCache.find(f->target);

                TransformComponent* target = it->second;
                float dx = target->Position.x - t->Position.x;
                float dy = target->Position.y - t->Position.y;
                float distance = std::sqrt(dx * dx + dy * dy);
                if (distance > target->Size.x + 0.01f) {
                    t->Position.x += (dx / distance) * v->vx * deltaTime;
                    t->Position.y += (dy / distance) * v->vy * deltaTime;
                    t->Rotation = std::atan2(dy, dx);
                }
            }
        );
    }

};

class GameLayer : public Layer {
public:
    void createSnake(Scene* scene, float speed)
    {
        int followerCount = 10;
        float size = 0.015;

		float rx = quickRandFloat(-1.0f, 1.0f);
		float ry = quickRandFloat(-1.0f, 1.0f);

        // create leader
        ID leaderEntity = scene->getWorld().createEntity();
        scene->getWorld().addComponents(
            leaderEntity,
            TransformComponent{ glm::vec2(rx, ry), glm::vec2(size, size) },
            SpriteComponent{ glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 0 },
            VelocityComponent{ speed, speed },
            Leader{0, false}
        );

        // create follwers for leader
        ID previousFollowerEntity = leaderEntity;
        for (size_t i = 0; i < followerCount; i++)
        {
            glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

            float randx = quickRandFloat(-1.0f, 1.0f);
            float randy = quickRandFloat(-1.0f, 1.0f);

            ID followerEntity = scene->getWorld().createEntity();
            scene->getWorld().addComponents(
                followerEntity,
                TransformComponent{ glm::vec2(randx, randy), glm::vec2(size, size) },
                SpriteComponent{ color, 0 },
                VelocityComponent{ speed * 10, speed * 10 },
                Follower{ previousFollowerEntity }
            );
            previousFollowerEntity = followerEntity;
        }
    }

    void onAttach() override
    {
        auto cameraController = std::make_shared<CameraController>(-1.0f, 1.0f, -1.0f, 1.0f);

        auto scene = getSceneManager().createScene("MainScene");
        scene->addSystem(std::make_unique<FollowerSystem>());
		scene->addSystem(std::make_unique<LeaderSystem>());
        scene->setCameraController(cameraController);

        getSceneManager().setActiveScene("MainScene");

		// create food
		for (int i = 0; i < 100; i++)
		{
			float randx = quickRandFloat(-1.0f, 1.0f);
			float randy = quickRandFloat(-1.0f, 1.0f);
			ID foodEntity = scene->getWorld().createEntity();
			scene->getWorld().addComponents(
				foodEntity,
				TransformComponent{ glm::vec2(randx, randy), glm::vec2(0.01, 0.01) },
				SpriteComponent{ glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 0 },
				Food{false}
			);
		}

        createSnake(scene.get(), 0.6);
        createSnake(scene.get(), 0.6);
        createSnake(scene.get(), 0.6);
        createSnake(scene.get(), 0.6);
    }
};

class MyApp : public Engine::EngineApp {
    AppConfig config() override { return AppConfig(800, 600, "Bouncing Balls Bigger"); }

    void setup() override
    {
        pushLayer(std::make_unique<GameLayer>());
    }
};

CREATE_ENGINE_APPLICATION(MyApp)
