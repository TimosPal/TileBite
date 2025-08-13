#include "physics/TilemapColliderGroup.hpp"

namespace Engine {

std::vector<CollisionData> TilemapColliderGroup::query(const AABB& collider, ID excludeID) const
{
	std::vector<CollisionData> results;

	results.push_back({ m_id, m_bounds });

	return results;
}

} // Engine