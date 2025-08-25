#include "physics/AABB.hpp"

#include "physics/Collider.hpp"

#include "physics/CollisionUtilities.hpp"

namespace Engine {

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

} // Engine