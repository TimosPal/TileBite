#ifndef COLLISION_UTILITIES_HPP
#define COLLISION_UTILITIES_HPP

#include <glm/glm.hpp>

#include "physics/AABB.hpp"
#include "physics/OBB.hpp"

namespace Engine {
namespace CollisionUtilities {

bool contains(const AABB& a, const OBB& b);
bool contains(const OBB& a, const AABB& b);

bool intersects(const AABB& a, const OBB& b);
bool intersects(const OBB& a, const AABB& b);

} // CollisionUtilities
} // Engine

#endif // !COLLISION_UTILITIES_HPP