#include "physics/OBB.hpp"

#include "physics/AABB.hpp"
#include "physics/CollisionUtilities.hpp"

namespace Engine {

AABB OBB::getAABB() const {
    glm::vec2 half = Size * 0.5f;

    // Corners relative to center
    glm::vec2 corners[4] = {
        {-half.x, -half.y},
        { half.x, -half.y},
        { half.x,  half.y},
        {-half.x,  half.y}
    };

    // Rotation matrix
    float cosR = cos(Rotation);
    float sinR = sin(Rotation);

    glm::vec2 minPt(FLT_MAX), maxPt(-FLT_MAX);

    for (auto& c : corners) {
        glm::vec2 rotated = {
            c.x * cosR - c.y * sinR,
            c.x * sinR + c.y * cosR
        };
        glm::vec2 world = rotated + Center;
        minPt = glm::min(minPt, world);
        maxPt = glm::max(maxPt, world);
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
    // TODO:
    return false;
}

bool OBB::intersects(const Collider& other) const
{
    // TODO:
    return false;
}

} // Engine