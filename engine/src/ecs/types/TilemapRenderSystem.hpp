#ifndef TILE_MAP_RENDER_SYSTEM_HPP
#define TILE_MAP_RENDER_SYSTEM_HPP

#include "ecs/ISystem.hpp"
#include "ecs/types/EngineComponents.hpp"
#include "renderer/Renderer2D.hpp"
#include "utilities/misc.hpp"

namespace Engine {

class TilemapRenderSystem : public ISystem {
public:
	virtual void update(float deltaTime) override
	{
		auto& renderer2D = getRenderer();
		auto& activeWorld = getSceneManager().getActiveScene()->getWorld();
		activeWorld.query<TilemapComponent, TransformComponent>().each([&](ID entityID, TilemapComponent* tilemapComp, TransformComponent* transformComp) {
			// TODO: Assumes resource is always loaded and valid, might need to retrieve a handle instead.
			auto resource = tilemapComp->getResource();
			uint8_t quadsCount = resource->getWidth() * resource->getHeight();
			renderer2D.drawTilemap(TilemapMesh{
				resource,
				transformComp
			});
		});
	}
};

} // Engine

#endif // !TILE_MAP_RENDER_SYSTEM_HPP
