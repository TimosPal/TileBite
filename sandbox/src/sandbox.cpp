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

#include <physics/aabb.hpp>

using namespace Engine;

class MainScene : public Scene {
    void onLoad() override
    {
        auto cameraController = std::make_shared<CameraController>(-1.0f, 1.0f, -1.0f, 1.0f);
        setCameraController(cameraController);

		auto floor = getWorld().createEntity();
        getWorld().addComponents(floor,
            TransformComponent{ {0.0f, -0.9f}, {2.0f, 0.3f}, 0.0f },
			SpriteComponent{ {1, 1, 1, 1}, 0 },
			AABB({ -0.5f, -0.5f }, { 0.5f, 0.5f })
        );

		float startX = -0.8f;
        float startY = 0.5f;
        for (size_t i = 0; i < 6; i++)
        {
            auto entID = getWorld().createEntity();
            getWorld().addComponents(entID,
                TransformComponent{ {startX, startY}, {0.2f, 0.2f}, 0.0f },
                SpriteComponent{ {1,0,0,1}, 0 },
                AABB({ -0.5f, -0.5f }, { 0.5f, 0.5f })
            );

			startX += 0.3f;
			startY -= 0.1f;
        }
    }
};

class GameLayer : public Layer {
public:

    void onAttach() override
    {
        auto scene = getSceneManager().createScene<MainScene>("MainScene");
        getSceneManager().setActiveScene(scene);
    }
};

class MyApp : public Engine::EngineApp {
    AppConfig config() override { return AppConfig(800, 600, "Testing demos"); }

    void setup() override
    {
        pushLayer(std::make_unique<GameLayer>());

        getLayer(DebugLayer::getName())->enable();
    }
};

CREATE_ENGINE_APPLICATION(MyApp)