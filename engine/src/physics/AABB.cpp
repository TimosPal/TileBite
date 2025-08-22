#include "physics/AABB.hpp"

#include "physics/CollisionUtilities.hpp"

namespace Engine {

bool AABB::contains(const OBB& other) const
{
	return CollisionUtilities::contains(*this, other);
}

bool AABB::intersects(const OBB& other) const
{
	return CollisionUtilities::intersects(*this, other);
}

bool AABB::contains(const Collider& other) const
{
	// TODO:
	return false;
}

bool AABB::intersects(const Collider& other) const
{
	// TODO:
	return false;
}

} // Engine