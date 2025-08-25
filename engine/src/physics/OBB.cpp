#include "physics/OBB.hpp"

#include "physics/AABB.hpp"
#include "physics/Collider.hpp"
#include "physics/CollisionUtilities.hpp"

namespace TileBite {

std::array<glm::vec2, 4> OBB::getCorners() const
{
    glm::vec2 half = Size * 0.5f;

    // Precompute rotation matrix components
    float cosR = cos(Rotation);
    float sinR = sin(Rotation);

    // Local-space corners relative to center (CCW order)
    std::array<glm::vec2, 4> corners = {
        glm::vec2(-half.x, -half.y),
        glm::vec2(half.x, -half.y),
        glm::vec2(half.x,  half.y),
        glm::vec2(-half.x,  half.y)
    };

    // Apply rotation (around center) and translation
    for (auto& c : corners) {
        glm::vec2 rotated(
            c.x * cosR - c.y * sinR,
            c.x * sinR + c.y * cosR
        );
        c = rotated + Center;
    }

    return corners;
}

AABB OBB::getAABB() const {
    glm::vec2 minPt(FLT_MAX), maxPt(-FLT_MAX);

    for (auto& c : getCorners()) {
        minPt = glm::min(minPt, c);
        maxPt = glm::max(maxPt, c);
    }

    return AABB(minPt, maxPt);
}

bool OBB::contains(const AABB& other) const {
	return CollisionUtilities::contains(other, *this);
}

bool OBB::intersects(const AABB& other) const {
	return CollisionUtilities::intersects(other, *this);
}

bool OBB::contains(const Collider& other) const
{
    return CollisionUtilities::contains(other, *this);
}

bool OBB::intersects(const Collider& other) const
{
    return CollisionUtilities::intersects(other, *this);
}

} // TileBite