#include "resources/types/TilemapResource.hpp"

#include "utilities/misc.hpp"

namespace Engine {

TilemapResource::TilemapResource(const std::string& resourceName, std::vector<Tile>& tiles, glm::vec2 dimensions, float tileSize, float atlasSize, ID atladID)
	: 
	Resource(resourceName, true), m_width(dimensions.x), m_height(dimensions.y), m_worldTileSize(tileSize), m_atlasTileSize(atlasSize), m_atlasID(atladID)
{
	for (size_t y = 0; y < dimensions.y; y++)
	{
		for (size_t x = 0; x < dimensions.x; x++)
		{
			Tile& tile = tiles[y * dimensions.y + x];

			auto quadVerts = makePackedTilemapQuad(
				x,
				y,
				tile.uIndex,
				tile.vIndex,
				tile.Color
			);
			m_vertices.insert(m_vertices.end(), quadVerts.begin(), quadVerts.end());
		}
	}

	m_isDirty = true;
}

bool TilemapResource::createImplementation()
{
	// TODO: laod from disk

   return true;
}

bool TilemapResource::destroyImplementation()
{
	// TODO: save on disk

    return true;
}

bool TilemapResource::isValid()
{
	return true;
}

} // Engine