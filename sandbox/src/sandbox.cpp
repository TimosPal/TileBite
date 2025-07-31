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

using namespace Engine;

class MainScene : public Scene {
    void onLoad() override
    {
        auto cameraController = std::make_shared<CameraController>(-1.0f, 1.0f, -1.0f, 1.0f);
        setCameraController(cameraController);
    }
};

class GameLayer : public Layer {
public:
    void onAttach() override
    {
        auto scene = getSceneManager().createScene<MainScene>("MainScene");
        getSceneManager().setActiveScene(scene);

        EventCallback<KeyPressedEvent> e([&](KeyPressedEvent& event) {
            LOG_INFO("Key: {}", event.getKeyCode());
		});
        getCoreEventDispatcher().subscribe(e);
    }
};

class MyApp : public Engine::EngineApp {
    AppConfig config() override { return AppConfig(800, 600, "sandbox"); }

    void setup() override
    {     
        pushLayer(std::make_unique<GameLayer>());
        
        //getLayer(DebugLayer::getName())->enable();
        //getLayer(GraphicsLayer::getName())->disable();
    }
};

CREATE_ENGINE_APPLICATION(MyApp)