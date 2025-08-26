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
#include <events/types/MouseEvent.hpp>
#include <events/types/KeyEvent.hpp>
#include <window/KeyCodes.hpp>

#include <physics/Ray2D.hpp>
#include <FastNoiseLite.h>

using namespace TileBite;

class MovingBoxSystem : public ISystem {
private:
    float timer = 0;
public:
    void onAttach() override
    {
        auto& world = getSceneManager().getActiveScene()->getWorld();

        //for (size_t i = 0; i < 1; i++)
        {
            ID box = world.createEntity();

            glm::vec2 randomPos = glm::vec2(
				quickRandFloat(-1.0f, 1.0f),
				quickRandFloat(-1.0f, 1.0f)
			);

            world.addComponents(
                box,
                TransformComponent{ glm::vec2(0.4, 0), glm::vec2(0.1f, 1.0f) },
                SpriteComponent(glm::vec4(0.7f, 0.0f, 0.0f, 1.0f), 0),
                OBBComponent(glm::vec2(0, 0), glm::vec2(1, 1), 0)
            );
        }

        //for (size_t i = 0; i < 1; i++)
        {
            ID box = world.createEntity();

            glm::vec2 randomPos = glm::vec2(
                quickRandFloat(-1.0f, 1.0f),
                quickRandFloat(-1.0f, 1.0f)
            );

            world.addComponents(
                box,
                TransformComponent{ glm::vec2(0.0, 0), glm::vec2(0.2f, 0.2f)},
                SpriteComponent(glm::vec4(0.7f, 0.2f, 0.0f, 1.0f), 0),
                AABBComponent(glm::vec2(-0.5), glm::vec2(0.5))
            );
        }
	}

    float pingPong(float t, float minVal = 0.1f, float maxVal = 0.2f) {
        float range = maxVal - minVal;
        return minVal + glm::abs(glm::mod(t, 2.0f * range) - range);
    }

    void update(float deltaTime) override
    {
		timer += deltaTime;

        auto& world = getSceneManager().getActiveScene()->getWorld();

        world.query<TransformComponent, SpriteComponent, OBBComponent>().each([&](ID id, TransformComponent* tr, SpriteComponent* spr, OBBComponent* obb) {
            tr->setRotation(tr->getRotation() + 0.2f * deltaTime);
            //tr->setSize(glm::vec2(pingPong(timer * 0.01f)));

            PhysicsEngine& physicsEngine = getSceneManager().getActiveScene()->getPhysicsEngine();
            OBB worldSpaceCol = obb->getCollider().toWorldSpace(tr->getPosition(), tr->getSize(), tr->getRotation());
            auto res = physicsEngine.query(worldSpaceCol, id);
        });

        Ray2D ray(glm::vec2(-2.0f, 0.0f), glm::vec2(1.0f, sin(timer * 0.3f) * 0.6f), 4.0f);

        getRenderer().drawLine(
            Line{
                ray.at(0.0f),
                ray.at(ray.getMaxT()),
                glm::vec4(1.0f)
            }
        );

		PhysicsEngine& physicsEngine = getSceneManager().getActiveScene()->getPhysicsEngine();
        std::optional<RayHitData> hit = physicsEngine.raycastClosest(ray);
        if(hit.has_value())
        {
            const RayHitData& hitData = hit.value();
            getRenderer().drawLine(
                Line{
                    ray.at(hitData.tmin),
                    ray.at(hitData.tmax),
                    glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)
                }
			);
            getRenderer().drawLine(
                Line{
                    ray.at(0),
                    ray.at(hitData.tmin),
                    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
                }
            );
            getRenderer().drawSquare(
                ray.at(hitData.tmin) - 0.01f,
                ray.at(hitData.tmin) + 0.01f,
                glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)
            );
            getRenderer().drawSquare(
                ray.at(hitData.tmax) - 0.01f,
                ray.at(hitData.tmax) + 0.01f,
                glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)
            );
		}
    }
};

class CameraSystem : public ISystem {
public:
    void update(float deltaTime) override
    {
        auto cam = getSceneManager().getActiveScene()->getCameraController();
		auto camPos = cam->getPosition();
	    glm::vec2 baseVec = glm::vec2(0.0f, 0.0f);
        if(getInputManager().isKeyDown(KeyCodes::KEY_A))
        {
			baseVec += glm::vec2(-1.0f, 0.0f);
		}
        if(getInputManager().isKeyDown(KeyCodes::KEY_D))
		{
            baseVec += glm::vec2(1.0f, 0.0f);
        }
        if (getInputManager().isKeyDown(KeyCodes::KEY_W))
        {
            baseVec += glm::vec2(0.0f, 1.0f);
        }
        if (getInputManager().isKeyDown(KeyCodes::KEY_S))
        {
            baseVec += glm::vec2(0.0f, -1.0f);
        }
        cam->setPosition(camPos + baseVec * deltaTime * 1.0f);

        float scrollY = getInputManager().getMouseScrollY();
        if(scrollY != 0.0f)
        {
            float delta = scrollY * 15.0f * deltaTime;
            float newZoom = std::clamp(cam->getZoom() + delta, 0.5f, 3.0f);
            cam->setZoom(newZoom);
		}
    }
};

class MainScene : public Scene {

    void onLoad() override
    {
        auto cameraController = std::make_shared<CameraController>();
        setCameraController(cameraController);
    }
};

class GameLayer : public Layer {
public: 
    void onAttach() override
    {

        auto scene = getSceneManager().createScene<MainScene>("MainScene");
        getSceneManager().setActiveScene(scene);

		getSystemManager().addSystem(std::make_unique<CameraSystem>());
		getSystemManager().addSystem(std::make_unique<MovingBoxSystem>());
    }
};

class MyApp : public TileBite::EngineApp {
    AppConfig config() override { return AppConfig(800, 600, "sandbox"); }

    void setup() override
    {     
        pushLayer(std::make_unique<GameLayer>());
    }
};

CREATE_ENGINE_APPLICATION(MyApp)