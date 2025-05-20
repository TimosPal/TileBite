#include <iostream>

#include <core/EntryPoint.hpp>
#include <core/EngineApp.hpp>

#include <utilities/Logger.hpp>
#include <events/types/WindowCloseEvent.hpp>
#include <layers/Layer.hpp>
#include <ecs/types/EngineComponents.hpp>

#include <random>

using namespace Engine;

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
		int unitCount = 10000;
		for (size_t i = 0; i < unitCount; i++)
		{
			float x = quickRandFloat();
			float y = quickRandFloat();

			float r = quickRandFloat(0.0f, 1.0f);
			float g = quickRandFloat(0.0f, 1.0f);
			float b = quickRandFloat(0.0f, 1.0f);

			World& world = getWorld();
			ID unit = world.createEntity();
			world.addComponents<SpriteComponent, TransformComponent>(
				unit,
				SpriteComponent{r, g, b},
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