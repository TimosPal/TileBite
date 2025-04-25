#include <iostream>

#include <core/EntryPoint.hpp>
#include <core/EngineApp.hpp>

#include <utilities/Logger.hpp>
#include <events/types/WindowCloseEvent.hpp>

#include <ecs/World.hpp>

struct CompA {
	int a = 5;
};
struct CompB {
	int a = 10;
};

using namespace Engine;
class MyApp : public Engine::EngineApp {
	void setup() override
	{
		Engine::World world;
		auto id = world.createEntity();
		world.addComponents(id, CompB(), CompA());
		{
			auto* comp = world.getComponent<CompA>(id);
			Engine::LOG_INFO("{}", comp->a);
		}
		{
			auto* comp = world.getComponent<CompB>(id);
			Engine::LOG_INFO("{}", comp->a);
		}

		onEvent(std::make_unique<WindowCloseEvent>());
	}
};

CREATE_ENGINE_APPLICATION(MyApp)