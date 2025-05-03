#include <iostream>

#include <core/EntryPoint.hpp>
#include <core/EngineApp.hpp>

#include <utilities/Logger.hpp>
#include <events/types/WindowCloseEvent.hpp>

#include <ecs/World.hpp>

#include <utilities/Bitset.hpp>

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
struct CompD {
	int val;
};

class MyApp : public Engine::EngineApp {
	void setup() override
	{
		Engine::World world;
		{
			auto id = world.createEntity();
			world.addComponents(id, CompA{ 1 });
		}

		{
			auto id = world.createEntity();
			world.addComponents(id, CompB{ 10 });
		}

		{
			auto id = world.createEntity();
			world.addComponents(id, CompA{ 2 }, CompB{ 10 });
		}

		{
			auto id = world.createEntity();
			world.addComponents(id, CompA{ 3 });
			world.addComponents(id, CompB{ 9 });
		}

		{
			auto id = world.createEntity();
			world.addComponents(id, CompC{ 0 });
		}

		{
			auto id = world.createEntity();
			world.addComponents(id, CompC{ 0 });
			world.addComponents(id, CompB{ 8 });
		}

		{
			auto id = world.createEntity();
			world.addComponents(id, CompA{ 4 });
			world.addComponents(id, CompC{ 0 });

			auto id2 = world.createEntity();
			world.addComponents(id2, CompA{ 5 });
			world.addComponents(id2, CompC{ 0 });

			world.addComponents(id, CompB{ 2 });
		}

		for (auto it : world.query<CompB, CompD>())
		{
			auto [compB, compD] = it;
			std::cout << "CompB: " << compB->val << ", CompD: " << compD->val << std::endl;
		}

		onEvent(std::make_unique<WindowCloseEvent>());
	}
};

CREATE_ENGINE_APPLICATION(MyApp)