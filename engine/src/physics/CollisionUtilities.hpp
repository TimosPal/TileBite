#ifndef COLLISION_UTILITIES_HPP
#define COLLISION_UTILITIES_HPP

#include <glm/glm.hpp>

#include "physics/AABB.hpp"
#include "physics/OBB.hpp"
#include "physics/Collider.hpp"

namespace TileBite {
namespace CollisionUtilities {

// TODO: template refactoring possible (?)

bool contains(const AABB& a, const OBB& b);
bool contains(const OBB& a, const AABB& b);

bool contains(const Collider& a, const OBB& b);
bool contains(const OBB& a, const Collider& b);
bool contains(const Collider& a, const AABB& b);
bool contains(const AABB& a, const Collider& b);

bool SATTest(const std::array<glm::vec2, 4>& points1, const std::array<glm::vec2, 4>& points2);

bool intersects(const AABB& a, const OBB& b);
bool intersects(const OBB& a, const AABB& b);

bool intersects(const Collider& a, const OBB& b);
bool intersects(const OBB& a, const Collider& b);
bool intersects(const Collider& a, const AABB& b);
bool intersects(const AABB& a, const Collider& b);

} // CollisionUtilities
} // TileBite

#endif // !COLLISION_UTILITIES_HPP