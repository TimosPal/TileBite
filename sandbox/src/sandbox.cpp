#include <iostream>

#include <core/EntryPoint.hpp>
#include <core/EngineApp.hpp>

#include <utilities/Logger.hpp>
#include <events/types/WindowCloseEvent.hpp>
#include <layers/Layer.hpp>
#include <ecs/types/EngineComponents.hpp>

#include <random>

using namespace Engine;

struct Health {
	int value;
};

inline float quickRandFloat(float min = -1.0f, float max = 1.0f) {
	return min + (max - min) * (rand() / float(RAND_MAX));
}


class UnitSystem : public ISystem {
public:
	float time;
	void update(World& world, float deltaTime) override
	{
		time += deltaTime;
		for (auto [transformComponent] : world.query<TransformComponent>())
		{
			float x = quickRandFloat();
			float y = quickRandFloat();
			transformComponent->x += (2.5f * cos(0.7 * time)) * (0.0f - transformComponent->x + x) * deltaTime;
			transformComponent->y += (2.5f * cos(0.7 * time)) * (0.0f - transformComponent->y + y) * deltaTime;
		}
	}
};

class GameLayer : public Layer {
public:
	GameLayer(World& world) : Layer(world) {}

	void onAttach() override
	{
		int unitCount = 100000;
		for (size_t i = 0; i < unitCount; i++)
		{
			float x = quickRandFloat();
			float y = quickRandFloat();

			World& world = getWorld();
			ID unit = world.createEntity();
			world.addComponents<Health, SpriteComponent, TransformComponent>(
				unit,
				Health{ (int)i },
				SpriteComponent(),
				TransformComponent{ x, y, 0.005f, 0.005f }
			);
		}

		addSystem(std::make_unique<UnitSystem>());
	}
};

class MyApp : public Engine::EngineApp {
	void setup() override
	{
		pushLayer(std::make_unique<GameLayer>(getWorld()));

		// onEvent(std::make_unique<WindowCloseEvent>());
	}
};

CREATE_ENGINE_APPLICATION(MyApp)