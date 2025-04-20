#include <iostream>
#include <core/EntryPoint.hpp>
#include <core/EngineApp.hpp>
#include <utilities/Logger.hpp>

#include <events/EventQueue.hpp>
#include <events/EventDispatcher.hpp>
#include <events/Event.hpp>
#include <events/EventCallback.hpp>

#include <utilities/Identifiable.hpp>

class CustomEvent : public Engine::Event {
	SETUP_ID(Event, CustomEvent)
public:
	CustomEvent() : Engine::Event(true) {}

	int x = 10;
};

class MyApp : public Engine::EngineApp {
	void setup() override
	{
		Engine::EventCallback<CustomEvent> ob([](const CustomEvent& event) {
			Engine::LOG_INFO("Test: {}", event.x);
		});
		subscribe<CustomEvent>(ob);

		CustomEvent e;		
		onEvent(std::make_unique<CustomEvent>(e));
		
		unsubscribe<CustomEvent>(ob);
	}
};

CREATE_ENGINE_APPLICATION(MyApp)