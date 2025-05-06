#include <iostream>

#include <core/EntryPoint.hpp>
#include <core/EngineApp.hpp>

#include <utilities/Logger.hpp>
#include <events/types/WindowCloseEvent.hpp>
#include <layers/Layer.hpp>
#include <ecs/types/EngineComponents.hpp>

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

class GameLayer : public Layer {
public:
	GameLayer(World& world) : Layer(world) {}

	void onAttach() override
	{
		int unitCount = 10;
		for (size_t i = 0; i < unitCount; i++)
		{
			World& world = getWorld();
			ID unit = world.createEntity();
			world.addComponents<Health, SpriteComponent, TransformComponent>(
				unit,
				Health{ (int)i },
				SpriteComponent(),
				TransformComponent()
			);
		}

		addSystem(std::make_unique<UnitSystem>());
	}
};

class MyApp : public Engine::EngineApp {
	void setup() override
	{
		pushLayer(std::make_unique<GameLayer>(getWorld()));

		//onEvent(std::make_unique<WindowCloseEvent>());
	}
};

CREATE_ENGINE_APPLICATION(MyApp)