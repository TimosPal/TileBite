#include <iostream>

#include <core/AppConfig.hpp>
#include <core/EntryPoint.hpp>
#include <core/EngineApp.hpp>
#include <utilities/Logger.hpp>

#include <events/EventQueue.hpp>
#include <events/EventDispatcher.hpp>
#include <events/Event.hpp>
#include <events/EventCallback.hpp>
#include <layers/Layer.hpp>

#include <utilities/Identifiable.hpp>

class CustomEvent : public Engine::Event {
	SETUP_ID(Event, CustomEvent)
public:
	CustomEvent() : Engine::Event(true) {}

	int x = 10;
};

class GameLayer : public Engine::Layer {
private:
	void onAttach() override
	{
		Engine::EventCallback<CustomEvent> ob([](CustomEvent& event) {
			Engine::LOG_INFO("Game: Test: {}", event.x);
			event.consume();
		});
		subscribe<CustomEvent>(ob);
	}
};

class UI : public Engine::Layer {
private:
	void onAttach() override
	{
		Engine::EventCallback<CustomEvent> ob([](CustomEvent& event) {
			Engine::LOG_INFO("UI: Test: {}", event.x);
		});
		subscribe<CustomEvent>(ob);
	}
};

class MyApp : public Engine::EngineApp {
	void setup() override
	{
		pushLayer(std::move(std::make_unique<GameLayer>()));
		pushLayer(std::move(std::make_unique<UI>()));

		CustomEvent e;		
		onEvent(std::make_unique<CustomEvent>(e));
	}
};

CREATE_ENGINE_APPLICATION(MyApp)