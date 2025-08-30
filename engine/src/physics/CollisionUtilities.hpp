#ifndef COLLISION_UTILITIES_HPP
#define COLLISION_UTILITIES_HPP

#include <glm/glm.hpp>

#include "physics/AABB.hpp"
#include "physics/OBB.hpp"
#include "physics/Collider.hpp"

namespace TileBite {
namespace CollisionUtilities {

// TODO: could use macro for symetric functions

// ====================================================
// Intersection tests between different collider types

bool intersects(const AABB& a, const OBB& b);
bool intersects(const OBB& a, const AABB& b);
bool intersects(const Circle& a, const AABB& b);
bool intersects(const AABB& a, const Circle& b);
bool intersects(const Circle& a, const OBB& b);
bool intersects(const OBB& a, const Circle& b);

bool intersects(const Collider& a, const OBB& b);
bool intersects(const OBB& a, const Collider& b);
bool intersects(const Collider& a, const AABB& b);
bool intersects(const AABB& a, const Collider& b);
bool intersects(const Collider& a, const Circle& b);
bool intersects(const Circle& a, const Collider& b);

bool SATTest(const std::array<glm::vec2, 4>& points1, const std::array<glm::vec2, 4>& points2);
bool SATTest(const AABB& aabb, const std::array<glm::vec2, 4>& points2);

} // CollisionUtilities
} // TileBite

#endif // !COLLISION_UTILITIES_HPP