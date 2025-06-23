#ifndef TILE_MAP_RENDER_SYSTEM_HPP
#define TILE_MAP_RENDER_SYSTEM_HPP

#include "ecs/ISystem.hpp"
#include "ecs/types/EngineComponents.hpp"
#include "renderer/Renderer2D.hpp"
#include "utilities/misc.hpp"

namespace Engine {

struct TilemapMesh
{
	std::vector<uint32_t> vertices;
	ID textureID;
	bool isDirty = true;
};

class TilemapRenderSystem : public ISystem {
public:
	TilemapRenderSystem(std::shared_ptr<Renderer2D> renderer2D) : m_renderer2D(renderer2D) {}

	void makeMesh(TilemapMesh& mesh, TilemapComponent* tilemapComp, TransformComponent* transformComp)
	{
		mesh.vertices.clear();
		mesh.textureID = tilemapComp->atlasID;
		for (size_t y = 0; y < tilemapComp->height; y++)
		{
			for (size_t x = 0; x < tilemapComp->width; x++)
			{
				Tile& tile = tilemapComp->getTile(x, y);

				auto quadVerts = makePackedTilemapQuad(
					x,
					y,
					tile.uIndex,
					tile.vIndex,
					tile.Color
				);
				mesh.vertices.insert(mesh.vertices.end(), quadVerts.begin(), quadVerts.end());
			}
		}

		mesh.isDirty = false;
	}

	virtual void update(float deltaTime) override
	{
		auto& activeWorld = getSceneManager()->getActiveScene()->getWorld();
		activeWorld.query<TilemapComponent, TransformComponent>().each([this](ID entityID, TilemapComponent* tilemapComp, TransformComponent* transformComp) {
			auto it = m_tilemapMeshes.find(entityID);
			if (it == m_tilemapMeshes.end()) {
				it = m_tilemapMeshes.emplace(entityID, TilemapMesh{ {}, 0, true}).first;
			}
			TilemapMesh& tilemapMesh = it->second;
			if(tilemapMesh.isDirty)
			{
				makeMesh(tilemapMesh, tilemapComp, transformComp);
			}

			uint8_t quadsCount = tilemapComp->height * tilemapComp->width;
			m_renderer2D->drawQuadMesh(QuadMesh{ tilemapMesh.vertices, tilemapMesh.textureID });
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
