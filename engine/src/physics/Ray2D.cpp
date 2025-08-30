#include "physics/Ray2D.hpp"

#include "utilities/assertions.hpp"

namespace TileBite {

bool Ray2D::intersect(const AABB& b, float& tmin, float& tmax) const {
    // Intersection with slabs in each axis
    glm::vec2 t0 = (b.Min - o) * invD;
    glm::vec2 t1 = (b.Max - o) * invD;
    // t when entering slab
    tmin = std::max(glm::min(t0.x, t1.x), glm::min(t0.y, t1.y));
    // t when exiting slab
    tmax = std::min(glm::max(t0.x, t1.x), glm::max(t0.y, t1.y));
    return tmax >= tmin;
}

bool Ray2D::intersect(const OBB& b, float& tmin, float& tmax) const {
    float cosR = cos(-b.Rotation);
    float sinR = sin(-b.Rotation);

    glm::vec2 rotatedRayDir(
        d.x * cosR - d.y * sinR,
        d.x * sinR + d.y * cosR
    );
    glm::vec2 localOrigin = o - b.Center;
    glm::vec2 rotatedRayOrigin(
        localOrigin.x * cosR - localOrigin.y * sinR,
        localOrigin.x * sinR + localOrigin.y * cosR
    );

    glm::vec2 min = -b.Size * 0.5f;
    glm::vec2 max = b.Size * 0.5f;

    Ray2D transformedRay(rotatedRayOrigin, rotatedRayDir, maxT);
    return transformedRay.intersect(AABB(min, max), tmin, tmax);
}

bool Ray2D::intersect(const Circle& b, float& tmin, float& tmax) const {
	// Solve for t in quadratic equation
    glm::vec2 f = o - b.Center;
    float a = glm::dot(d, d);
    float b_coef = 2.0f * glm::dot(f, d);
    float c = glm::dot(f, f) - b.Radius * b.Radius;
    float disc = b_coef * b_coef - 4 * a * c;

    // No sollutions found
    if (disc < 0.0f) return false;

    float sqrtDisc = std::sqrt(disc);
    float t1 = (-b_coef - sqrtDisc) / (2 * a);
    float t2 = (-b_coef + sqrtDisc) / (2 * a);

    tmin = std::min(t1, t2);
    tmax = std::max(t1, t2);

    if (tmax < 0.0f) return false; // both behind ray
    if (tmin < 0.0f) tmin = tmax; // ray starts inside circle

    return true;
}

bool Ray2D::intersect(const Collider& other, float& tmin, float& tmax) const {
    switch (other.Type) {
    case Collider::ColliderType::AABB: return intersect(other.AABBCollider, tmin, tmax);
    case Collider::ColliderType::OBB:  return intersect(other.OBBCollider, tmin, tmax);
    case Collider::ColliderType::Circle:  return intersect(other.CircleCollider, tmin, tmax);
    default: ASSERT_FALSE("Unknown collider type");
    }
    return false;
}

} // TileBite