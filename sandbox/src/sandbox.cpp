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
		Engine::Event<int> e;
		Engine::IEvent& ie = e;
		Engine::LOG_INFO("{}", ie.getInstanceID());

		Engine::EventCallback<Engine::Event<int>> ob(foo);
		Engine::EventQueue queue;

		Engine::EventDispatcher dispatcher;
		dispatcher.subscribe<Engine::Event<int>>(ob);

		queue.add<Engine::Event<int>>();

		queue.dispatchAll(dispatcher);

		dispatcher.unsubscribe<Engine::Event<int>>(ob);
	}
};

CREATE_ENGINE_APPLICATION(MyApp)