#include <iostream>
#include <core/EntryPoint.hpp>
#include <core/EngineApp.hpp>
#include <utilities/Logger.hpp>

#include <events/EventQueue.hpp>
#include <events/EventDispatcher.hpp>
#include <events/Event.hpp>
#include <events/EventCallback.hpp>

void foo(const Engine::Event<int>& e) { Engine::LOG_INFO("Test"); }

class MyApp : public Engine::EngineApp {
	void setup() override
	{
		using EventType = Engine::Event<int>;
		EventType e;

		
		Engine::EventCallback<Engine::Event<int>> ob(foo);
		subscribe<Engine::Event<int>>(ob);
		onEvent(std::make_unique<EventType>(e));
		unsubscribe<Engine::Event<int>>(ob);
	}
};

CREATE_ENGINE_APPLICATION(MyApp)