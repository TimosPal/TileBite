#include <iostream>

#include <core/EntryPoint.hpp>
#include <core/EngineApp.hpp>

#include <utilities/Logger.hpp>
#include <events/types/WindowCloseEvent.hpp>
#include "layers/Layer.hpp"

using namespace Engine;

struct Health {
	int value;
};

class UnitSystem : public ISystem {
public:
	void update(World& world, float deltaTime) override
	{
		for (auto [healthComp] : world.query<Health>())
		{
			LOG_INFO("Unit Health: {}", healthComp->value);
		}
	}
};

class MyApp : public Engine::EngineApp {
	void setup() override
	{
		World& world = getWorld();
		int unitCount = 10;
		for (size_t i = 0; i < unitCount; i++)
		{
			ID unit = world.createEntity();
			world.addComponents<Health>(unit, Health{ (int)i });
		}

		addSystem(std::make_unique<UnitSystem>());

		//onEvent(std::make_unique<WindowCloseEvent>());
	}
};

CREATE_ENGINE_APPLICATION(MyApp)