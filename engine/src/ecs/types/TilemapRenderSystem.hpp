#ifndef TILE_MAP_RENDER_SYSTEM_HPP
#define TILE_MAP_RENDER_SYSTEM_HPP

#include "ecs/ISystem.hpp"
#include "ecs/types/EngineComponents.hpp"
#include "renderer/Renderer2D.hpp"
#include "utilities/misc.hpp"

#include "core/EngineApp.hpp"

namespace TileBite {

class TilemapRenderSystem : public ISystem {
public:
	virtual void update(float deltaTime) override
	{
		auto& renderer2D = EngineApp::getInstance()->getRenderer();
		auto& activeWorld = EngineApp::getInstance()->getSceneManager().getActiveScene()->getWorld();
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

} // TileBite

#endif // !TILE_MAP_RENDER_SYSTEM_HPP
