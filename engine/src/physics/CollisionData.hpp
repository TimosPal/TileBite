#ifndef COLLISION_DATA_HPP
#define COLLISION_DATA_HPP

#include "physics/AABB.hpp"
#include "core/types.hpp"

namespace Engine {

// Stores data about a collision, including the ID of the collider and its AABB in world space.
struct CollisionData {
	ID id;
	AABB Collider;
};

} // Engine

#endif // !COLLISION_DATA_HPP