#include "resources/types/TilemapResource.hpp"

#include "utilities/misc.hpp"

namespace Engine {

TilemapResource::TilemapResource(
	const std::string& resourceName,
	std::vector<Tile>& tiles,
	glm::vec2 dimensions,
	glm::vec2 tileSize,
	glm::vec2 atlasSize,
	glm::vec2 atlasDim,
	ID atlasID)
	: 
	Resource(resourceName, true),
	m_width(dimensions.x), m_height(dimensions.y), m_worldTileSize(tileSize), m_atlasTileSize(atlasSize), m_atlasID(atlasID), m_atlasDim(atlasDim)
{
	m_vertices.resize(dimensions.x * dimensions.y * PACKED_TILEMAP_QUAD_BYTES);
	for (size_t y = 0; y < dimensions.y; y++)
	{
		for (size_t x = 0; x < dimensions.x; x++)
		{
			Tile& tile = tiles[y * dimensions.x + x];

			auto quadVerts = makePackedTilemapQuad(
				x,
				y,
				tile.uIndex,
				tile.vIndex,
				tile.Color
			);

			setTile(tile, x, y);
		}
	}

	// Add a change to the whole tilemap so it is updated on the first draw call.
	m_bytesChanges.clear(); // Clear previous changes, only need one batched update.
	m_bytesChanges.push_back(BytesChange{0, (uint32_t)m_vertices.size() * sizeof(uint32_t)});
}

void TilemapResource::setTile(Tile tile, uint8_t xIndex, uint8_t yIndex)
{
	auto quadVerts = makePackedTilemapQuad(
		xIndex,
		yIndex,
		tile.uIndex,
		tile.vIndex,
		tile.Color
	);

	uint32_t elementOffset = (yIndex * m_width + xIndex) * quadVerts.size();
	uint32_t byteOffset = elementOffset * sizeof(uint32_t);
	uint32_t size = PACKED_TILEMAP_QUAD_BYTES * sizeof(uint32_t);

	memcpy(
		m_vertices.data() + elementOffset,
		quadVerts.data(),
		size
	);

	m_bytesChanges.push_back(BytesChange{ byteOffset, size });
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