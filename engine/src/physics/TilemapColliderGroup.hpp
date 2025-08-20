#ifndef TILEMAP_COLLIDER_GROUP_HPP
#define TILEMAP_COLLIDER_GROUP_HPP

#include "physics/AABB.hpp"
#include "physics/CollisionData.hpp"
#include "physics/Ray2D.hpp"
#include "ecs/types/EngineComponents.hpp"
#include "utilities/Bitset.hpp"

namespace Engine {

class TilemapColliderGroup {
public:
	TilemapColliderGroup() = default;
	TilemapColliderGroup(ID tilemapID, const AABB& bounds, glm::vec2 tilemapSize, glm::vec2 tileSize, Bitset solidTiles)
		: m_bounds(bounds), m_tiles(solidTiles),
		tilemapSize(tilemapSize), tileSize(tileSize), m_id(tilemapID)
	{}

	std::vector<CollisionData> query(const AABB& collider) const;
	std::vector<RayHitData> raycastAll(const Ray2D& ray) const;
	std::optional<RayHitData> raycastClosest(const Ray2D& ray) const;
private:
	AABB m_bounds; // The bounding box of the tilemap collider group
	glm::vec2 tilemapSize, tileSize;
	
	Bitset m_tiles; // Bitset representing the tiles in the group
	ID m_id = INVALID_ID; // Unique ID for the tilemap collider group

	std::vector<RayHitData> ADDSearch(const Ray2D& ray, bool stopAtFirst) const;
};

} // Engine

#endif // TILEMAP_COLLIDER_GROUP_HPP