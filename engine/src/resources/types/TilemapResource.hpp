#ifndef TILE_MAP_RESOURCE_HPP
#define TILE_MAP_RESOURCE_HPP

#include "core/pch.hpp"
#include "resources/Resource.hpp"

#include "ecs/types/EngineComponents.hpp"

namespace Engine {

class TilemapResource : public Resource<TilemapResource> {
	SETUP_ID(Resource, TilemapResource)
public:
	TilemapResource(const std::string& resourceName, std::vector<Tile>& tiles, glm::vec2 dimensions, float tileSize, float atlasSize, ID atladID);

	std::vector<uint32_t>& getData() { return m_vertices; }
	virtual bool isValid() override;

	// Getters
	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }
	float getWorldTileSize() const { return m_worldTileSize; }
	float getAtlasTileSize() const { return m_atlasTileSize; }
	ID getAtlasID() const { return m_atlasID; }
	bool& GetIsDirty() { return m_isDirty; }

	TilemapResource(TilemapResource&&) noexcept = default;
	TilemapResource& operator=(TilemapResource&&) noexcept = default;
	TilemapResource(const TilemapResource&) = delete;
	TilemapResource& operator=(const TilemapResource&) = delete;
private:
	virtual bool createImplementation() override;
	virtual bool destroyImplementation() override;

	int m_width;
	int m_height;
	float m_worldTileSize;
	float m_atlasTileSize;

	ID m_atlasID;

	std::vector<uint32_t> m_vertices;
	bool m_isDirty;
};

} // Engine

#endif // !TILE_MAP_RESOURCE_HPP
