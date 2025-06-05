#ifndef TILE_MAP_RENDER_SYSTEM_HPP
#define TILE_MAP_RENDER_SYSTEM_HPP

#include "ecs/ISystem.hpp"
#include "ecs/types/EngineComponents.hpp"
#include "renderer/Renderer2D.hpp"

namespace Engine {

inline float quickRandFloat(float min = -1.0f, float max = 1.0f) {
	return min + (max - min) * (rand() / float(RAND_MAX));
}

class TilemapRenderSystem : public ISystem {
public:
	TilemapRenderSystem(std::shared_ptr<Renderer2D> renderer2D) : m_renderer2D(renderer2D) {}

	virtual void update(float deltaTime) override
	{
		auto& activeWorld = getSceneManager()->getActiveScene()->getWorld();
		activeWorld.query<TilemapComponent, TransformComponent>().each([this](ID entityID, TilemapComponent* tilemapComp, TransformComponent* transformComp) {
			for (size_t y = 0; y < tilemapComp->height; y++)
			{
				for (size_t x = 0; x < tilemapComp->width; x++)
				{
					Tile& tile = tilemapComp->getTile(x, y);
					tile.transform.Position = glm::vec2(
						transformComp->Position.x + x * tilemapComp->tileSize,
						transformComp->Position.y + y * tilemapComp->tileSize
					);
					tile.transform.Size = glm::vec2(tilemapComp->tileSize, tilemapComp->tileSize);
					
					m_renderer2D->drawQuad(SpriteQuad{ &tile.transform, &tile.sprite });
				}
			}
		});
	}

private:
	std::shared_ptr<Renderer2D> m_renderer2D;
};

} // Engine

#endif // !TILE_MAP_RENDER_SYSTEM_HPP
