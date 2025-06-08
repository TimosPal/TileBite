#ifndef TILE_MAP_RENDER_SYSTEM_HPP
#define TILE_MAP_RENDER_SYSTEM_HPP

#include "ecs/ISystem.hpp"
#include "ecs/types/EngineComponents.hpp"
#include "renderer/Renderer2D.hpp"
#include "utilities/misc.hpp"

namespace Engine {

struct TilemapMesh
{
	std::vector<float> vertices;
	bool isDirty = true;
};

class TilemapRenderSystem : public ISystem {
public:
	TilemapRenderSystem(std::shared_ptr<Renderer2D> renderer2D) : m_renderer2D(renderer2D) {}

	std::vector<float> makeMesh(TilemapComponent* tilemapComp, TransformComponent* transformComp)
	{
		std::vector<float> vertices;
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

				auto quadVerts = makeSpriteQuadVertices(&tile.transform, &tile.sprite);
				vertices.insert(vertices.end(), quadVerts.begin(), quadVerts.end());
			}
		}

		return vertices;
	}

	virtual void update(float deltaTime) override
	{
		auto& activeWorld = getSceneManager()->getActiveScene()->getWorld();
		activeWorld.query<TilemapComponent, TransformComponent>().each([this](ID entityID, TilemapComponent* tilemapComp, TransformComponent* transformComp) {
			// If the tilemap mesh is dirty or not found, create or update it
			TilemapMesh& tilemapMesh = m_tilemapMeshes[entityID];
			if(tilemapMesh.isDirty)
			{
				tilemapMesh.vertices = makeMesh(tilemapComp, transformComp);
				tilemapMesh.isDirty = false;
			}		
		});
	}

private:
	// TODO: not proper sollution, but for now we store tilemaps in a map <entityID, tilemapMesh>
	// Later on we should store meshes in a resource manager. Also currently ssupporting only fixed size tilemaps.
	std::unordered_map<ID, TilemapMesh> m_tilemapMeshes;

	std::shared_ptr<Renderer2D> m_renderer2D;
};

} // Engine

#endif // !TILE_MAP_RENDER_SYSTEM_HPP
