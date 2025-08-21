#include "physics/CollisionUtilities.hpp"

namespace Engine {
namespace CollisionUtilities {

bool contains(const AABB& a, const OBB& b)
{
	// TODO:
	return false;
}

bool contains(const OBB& a, const AABB& b) {
	return contains(b, a); // reuse logic
}

bool intersects(const AABB& a, const OBB& b)
{
	// TODO:
	return false;
}

bool intersects(const OBB& a, const AABB& b) {
	return intersects(b, a); // reuse logic
}

} // CollisionUtilities
} // Engine