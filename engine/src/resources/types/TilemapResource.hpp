#ifndef TILE_MAP_RESOURCE_HPP
#define TILE_MAP_RESOURCE_HPP

#include "core/pch.hpp"
#include "resources/Resource.hpp"

namespace Engine {

class TilemapResource : public Resource<TilemapResource> {
	SETUP_ID(Resource, TilemapResource)
public:
	TilemapResource(const std::string& resourceName, std::vector<Tile>& tiles);

	std::vector<Tile>& getData() { return m_tiles; }
	virtual bool isValid() override;

	TilemapResource(TilemapResource&&) noexcept = default;
	TilemapResource& operator=(TilemapResource&&) noexcept = default;
	TilemapResource(const TilemapResource&) = delete;
	TilemapResource& operator=(const TilemapResource&) = delete;
private:
	virtual bool createImplementation() override;
	virtual bool destroyImplementation() override;

	std::vector<Tile> m_tiles;
};

} // Engine

#endif // !TILE_MAP_RESOURCE_HPP
