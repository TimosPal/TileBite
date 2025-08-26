#include "physics/AABB.hpp"

#include "physics/Collider.hpp"

#include "physics/CollisionUtilities.hpp"

namespace TileBite {

AABB AABB::toWorldSpace(glm::vec2 position, glm::vec2 size, float radians) const
{
    // TODO: rotation utility math func
    if (radians == 0.0f) {
        glm::vec2 min = Min * size + position;
        glm::vec2 max = Max * size + position;
        return AABB(min, max);
    }

    // rotation math
    float c = cos(radians);
    float s = sin(radians);

    glm::vec2 centerPoint = (Max + Min) * 0.5f;
    glm::vec2 rotatedCenter(
        centerPoint.x * c - centerPoint.y * s,
        centerPoint.x * s + centerPoint.y * c
    );

    glm::vec2 localMin = rotatedCenter - (Max - Min) * 0.5f;
    glm::vec2 localMax = rotatedCenter + (Max - Min) * 0.5f;

    glm::vec2 min = localMin * size + position;
    glm::vec2 max = localMax * size + position;

    return AABB(min, max);
}

std::array<glm::vec2, 4> AABB::getCorners() const
{
    // Order: bottom-left, bottom-right, top-right, top-left
    return {
        Min,
        {Max.x, Min.y},
        Max,
        {Min.x, Max.y}
    };
}

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
	return CollisionUtilities::contains(*this, other);
}

bool AABB::intersects(const Collider& other) const
{
	return CollisionUtilities::intersects(*this, other);
}

} // TileBite