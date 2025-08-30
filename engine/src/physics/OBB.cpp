#include "physics/OBB.hpp"

#include "physics/AABB.hpp"
#include "physics/Collider.hpp"
#include "physics/CollisionUtilities.hpp"

namespace TileBite {

OBB OBB::toWorldSpace(glm::vec2 position, glm::vec2 size, float radians) const {
    // TODO: rotation utility math func
    float c = cos(radians);
    float s = sin(radians);

    glm::vec2 localCenter = Center * size;
    glm::vec2 rotatedCenter(
        localCenter.x * c - localCenter.y * s,
        localCenter.x * s + localCenter.y * c
    );

    OBB worldSpaceOBB(
        rotatedCenter + position,
        Size * size,
        Rotation + radians
    );

    return worldSpaceOBB;
}

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

AABB OBB::getBoundingBox() const {
    glm::vec2 minPt(FLT_MAX), maxPt(-FLT_MAX);

    for (auto& c : getCorners()) {
        minPt = glm::min(minPt, c);
        maxPt = glm::max(maxPt, c);
    }

    return AABB(minPt, maxPt);
}

bool OBB::intersects(const AABB& other) const
{
	return CollisionUtilities::intersects(other, *this);
}

bool OBB::intersects(const Collider& other) const
{
    return CollisionUtilities::intersects(other, *this);
}

bool OBB::intersects(const Circle& other) const
{
    return CollisionUtilities::intersects(other, *this);
}

bool OBB::intersects(const OBB& other) const
{
    return CollisionUtilities::SATTest(getCorners(), other.getCorners());
}

} // TileBite