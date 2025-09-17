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
        auto activeScene = EngineApp::getInstance()->getSceneManager().getActiveScene();
        auto& world = activeScene->getWorld();
        auto& graph = activeScene->getSceneGraph();

        ID boxLeft = world.createEntity();
		TransformComponent boxLeftTr = { glm::vec2(0.0, 0), glm::vec2(0.2f) };
        
        world.addComponents(
            boxLeft,
            boxLeftTr,
            SpriteComponent(glm::vec4(0.7f, 0.2f, 0.0f, 1.0f), 0),
            OBBComponent(glm::vec2(0, 0), glm::vec2(1, 1), 0)
        );

		float xMargin = 0.21f;
		ID previousID = boxLeft;
        for (size_t i = 0; i < 10; i++)
        {
            ID boxRight = world.createEntity();
            TransformComponent boxRightTr = { glm::vec2(xMargin + xMargin * i, 0), glm::vec2(0.2f) };
            //TransformComponent boxRightTr = { glm::vec2(xMargin, 0), glm::vec2(1.0f) };

            //linkEntity(&boxLeftTr, &boxRightTr);

            world.addComponents(
                boxRight,
                boxRightTr,
                SpriteComponent(glm::vec4(0.7f, 0.0f, 0.0f, 1.0f), 0),
                OBBComponent(glm::vec2(0, 0), glm::vec2(1, 1), 0)
			    ,ParentComponent(previousID)
            );

			previousID = boxRight;
            boxLeftTr = boxRightTr;
        }
		
	}

    float pingPong(float t, float minVal = 0.1f, float maxVal = 0.2f) {
        float range = maxVal - minVal;
        return minVal + glm::abs(glm::mod(t, 2.0f * range) - range);
    }

    void update(float deltaTime) override
    {
		timer += deltaTime;

        auto activeScene = EngineApp::getInstance()->getSceneManager().getActiveScene();
        auto& world = activeScene->getWorld();

		World::TypePack<ParentComponent> excludedTypes;
        world.query<TransformComponent, SpriteComponent, OBBComponent>(excludedTypes).each([&](ID id, TransformComponent* tr, SpriteComponent* spr, OBBComponent* obb) {
            tr->setRotation(tr->getRotation() + 0.2f * deltaTime);
            //tr->setSize(glm::vec2(pingPong(timer * 0.01f)));
			//tr->setPosition(glm::vec2(tr->getPosition().x, sin(timer + id * 0.4f) * 0.5f));

            PhysicsEngine& physicsEngine = activeScene->getPhysicsEngine();
            OBB worldSpaceCol = obb->getCollider().toWorldSpace(tr->getPosition(), tr->getSize(), tr->getRotation());
            auto res = physicsEngine.query(worldSpaceCol, id);
            if(res.size() > 0)
            {
                spr->Color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
            }
            else
            {
                spr->Color = glm::vec4(0.7f, 0.0f, 0.0f, 1.0f);
			}
        });

        Ray2D ray(glm::vec2(-2.0f, 0.0f), glm::vec2(1.0f, sin(timer * 0.3f) * 0.6f), 4.0f);
        auto& renderer = EngineApp::getInstance()->getRenderer();

        renderer.drawLine(
            Line{
                ray.at(0.0f),
                ray.at(ray.getMaxT()),
                glm::vec4(1.0f)
            }
        );

        PhysicsEngine& physicsEngine = activeScene->getPhysicsEngine();
        std::optional<RayHitData> hit = physicsEngine.raycastClosest(ray);
        if (hit.has_value())
        {
            const RayHitData& hitData = hit.value();
            renderer.drawLine(
                Line{
                    ray.at(hitData.tmin),
                    ray.at(hitData.tmax),
                    glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)
                }
            );
            renderer.drawLine(
                Line{
                    ray.at(0),
                    ray.at(hitData.tmin),
                    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
                }
            );
            renderer.drawSquare(
                ray.at(hitData.tmin) - 0.01f,
                ray.at(hitData.tmin) + 0.01f,
                glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)
            );
            renderer.drawSquare(
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
        auto cam = EngineApp::getInstance()->getSceneManager().getActiveScene()->getCameraController();
        auto& inputManager = EngineApp::getInstance()->getInputManager();
		auto camPos = cam->getPosition();
	    glm::vec2 baseVec = glm::vec2(0.0f, 0.0f);
        if(inputManager.isKeyDown(KeyCodes::KEY_A))
        {
            baseVec += glm::vec2(-1.0f, 0.0f);
        }
        if (inputManager.isKeyDown(KeyCodes::KEY_D))
        {
            baseVec += glm::vec2(1.0f, 0.0f);
        }
        if (inputManager.isKeyDown(KeyCodes::KEY_W))
        {
            baseVec += glm::vec2(0.0f, 1.0f);
        }
        if (inputManager.isKeyDown(KeyCodes::KEY_S))
        {
            baseVec += glm::vec2(0.0f, -1.0f);
        }
        cam->setPosition(camPos + baseVec * deltaTime * 1.0f);

        float scrollY = inputManager.getMouseScrollY();
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
        auto scene = EngineApp::getInstance()->getSceneManager().createScene<MainScene>("MainScene");
        EngineApp::getInstance()->getSceneManager().setActiveScene(scene);

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