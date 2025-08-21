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

} // Engine