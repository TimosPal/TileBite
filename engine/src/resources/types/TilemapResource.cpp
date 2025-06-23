#include "resources/types/TilemapResource.hpp"

namespace Engine {

TilemapResource::TilemapResource(const std::string& resourceName, std::vector<Tile>& tiles)
	: 
    Resource(resourceName, true),
    m_tiles(tiles)
{}

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