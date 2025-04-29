#include <iostream>

#include <core/EntryPoint.hpp>
#include <core/EngineApp.hpp>

#include <utilities/Logger.hpp>
#include <events/types/WindowCloseEvent.hpp>

#include <ecs/World.hpp>

using namespace Engine;

struct CompA {
	int val;
};
struct CompB {
	int val;
};
struct CompC {
	int val;
};

class MyApp : public Engine::EngineApp {
	void setup() override
	{
		Engine::World world;

		{
			auto id = world.createEntity();
			world.addComponents(id, CompA{ 10 });
		}

		{
			auto id = world.createEntity();
			world.addComponents(id, CompB{ 10 });
		}

		{
			auto id = world.createEntity();
			world.addComponents(id, CompA{ 0 }, CompB{ 10 });
		}

		{
			auto id = world.createEntity();
			world.addComponents(id, CompA{ 0 });
			world.addComponents(id, CompB{ 0 });
		}

		{
			auto id = world.createEntity();
			world.addComponents(id, CompC{ 0 });
		}

		{
			auto id = world.createEntity();
			world.addComponents(id, CompC{ 0 });
			world.addComponents(id, CompB{ 0 });
		}

		{
			auto id = world.createEntity();
			world.addComponents(id, CompA{ 0 });
			world.addComponents(id, CompC{ 0 });

			auto id2 = world.createEntity();
			world.addComponents(id2, CompA{ 0 });
			world.addComponents(id2, CompC{ 0 });

			world.addComponents(id, CompB{ 0 });
		}


		onEvent(std::make_unique<WindowCloseEvent>());
	}
};

CREATE_ENGINE_APPLICATION(MyApp)