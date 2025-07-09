#ifndef TILE_MAP_RESOURCE_HPP
#define TILE_MAP_RESOURCE_HPP

#include "core/pch.hpp"
#include "resources/Resource.hpp"

#include "ecs/types/EngineComponents.hpp"

namespace Engine {

struct BytesChange {
	uint32_t Offset;
	uint32_t Size;
};

class TilemapResource : public Resource<TilemapResource> {
	SETUP_ID(Resource, TilemapResource)
public:
	// TODO: refactor
	TilemapResource(
		const std::string& resourceName,
		std::vector<Tile>& tiles,
		glm::vec2 dimensions,
		glm::vec2 tileSize,
		glm::vec2 atlasSize,
		glm::vec2 atlasDim,
		ID atladID
	);

	std::vector<uint32_t>& getData() { return m_vertices; }
	virtual bool isValid() override;

	// Getters
	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }
	glm::vec2 getWorldTileSize() const { return m_worldTileSize; }
	glm::vec2 getAtlasTileSize() const { return m_atlasTileSize; }
	glm::vec2 getAtlasDim() const { return m_atlasDim; }
	ID getAtlasID() const { return m_atlasID; }
	bool isDirty() { return !m_bytesChanges.empty(); }
	void resetChangesList() { m_bytesChanges.clear(); }
	const std::vector<BytesChange>& getBytesChanges() const { return m_bytesChanges; }

	void mergeBytesChanges();

	// Setters
	void setTile(Tile tile, uint8_t xIndex, uint8_t yIndex);

	TilemapResource(TilemapResource&&) noexcept = default;
	TilemapResource& operator=(TilemapResource&&) noexcept = default;
	TilemapResource(const TilemapResource&) = delete;
	TilemapResource& operator=(const TilemapResource&) = delete;
private:
	virtual bool createImplementation() override;
	virtual bool destroyImplementation() override;

	int m_width;
	int m_height;
	glm::vec2 m_worldTileSize;
	glm::vec2 m_atlasTileSize;
	glm::vec2 m_atlasDim;


	ID m_atlasID;

	std::vector<uint32_t> m_vertices;

	std::vector<BytesChange> m_bytesChanges;
};

} // Engine

#endif // !TILE_MAP_RESOURCE_HPP
