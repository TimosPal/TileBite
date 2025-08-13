#include "ecs/types/EngineComponents.hpp"
#include "resources/types/TilemapResource.hpp"

namespace Engine {

// Needs to be in CPP file to avoid circular dependency issues
void TilemapComponent::setTile(Tile tile, uint8_t xIndex, uint8_t yIndex)
{
	if (TilemapResourcePtr)
	{
		TilemapResourcePtr->setTile(tile, xIndex, yIndex);
		BaseComponent::setDirty(true);
	}
}

} // Engine