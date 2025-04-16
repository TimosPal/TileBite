#include <iostream>

#include <core/EntryPoint.hpp>
#include <core/EngineApp.hpp>

class MyApp : public Engine::EngineApp {
	void setup() override
	{

	}
};

CREATE_ENGINE_APPLICATION(MyApp)