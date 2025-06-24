#ifndef TILE_MAP_RENDER_SYSTEM_HPP
#define TILE_MAP_RENDER_SYSTEM_HPP

#include "ecs/ISystem.hpp"
#include "ecs/types/EngineComponents.hpp"
#include "renderer/Renderer2D.hpp"
#include "utilities/misc.hpp"

namespace Engine {

class TilemapRenderSystem : public ISystem {
public:
	TilemapRenderSystem(std::shared_ptr<Renderer2D> renderer2D) : m_renderer2D(renderer2D) {}

	virtual void update(float deltaTime) override
	{
		auto& activeWorld = getSceneManager()->getActiveScene()->getWorld();
		activeWorld.query<TilemapComponent, TransformComponent>().each([this](ID entityID, TilemapComponent* tilemapComp, TransformComponent* transformComp) {
			// TODO: Assumes resource is always loaded and valid, might need to retrieve a handle instead.
			auto resource = tilemapComp->TilemapResource;
			auto& vertices = resource->getData();
			uint8_t quadsCount = resource->getWidth() * resource->getHeight();
			m_renderer2D->drawQuadMesh(QuadMesh{ &vertices, resource->getAtlasID(), resource->GetIsDirty() });
		});
	}

private:
	std::shared_ptr<Renderer2D> m_renderer2D;
};

} // Engine

#endif // !TILE_MAP_RENDER_SYSTEM_HPP
