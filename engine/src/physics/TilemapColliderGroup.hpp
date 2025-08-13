#ifndef TILEMAP_COLLIDER_GROUP_HPP
#define TILEMAP_COLLIDER_GROUP_HPP

#include "physics/AABB.hpp"
#include "ecs/types/EngineComponents.hpp"

namespace Engine {

class TilemapColliderGroup {
public:
	TilemapColliderGroup() = default;
	TilemapColliderGroup(const AABB& bounds)
		: m_bounds(bounds) 
	{}

	bool isColliding(const AABB& collider) const { return m_bounds.intersects(collider); }
private:
	AABB m_bounds; // The bounding box of the tilemap collider group
};

} // Engine

#endif // TILEMAP_COLLIDER_GROUP_HPP