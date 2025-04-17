#include <iostream>

#include <core/EntryPoint.hpp>
#include <core/EngineApp.hpp>
#include <utilities/Logger.hpp>

class MyApp : public Engine::EngineApp {
	void setup() override
	{
		Engine::LOG_INFO("Hi!");
	}
};

CREATE_ENGINE_APPLICATION(MyApp)