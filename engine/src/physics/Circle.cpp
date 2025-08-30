#include "physics/Circle.hpp"

#include "physics/Collider.hpp"
#include "physics/CollisionUtilities.hpp"
#include "utilities/Logger.hpp"

namespace TileBite{

Circle Circle::toWorldSpace(glm::vec2 position, glm::vec2 size, float radians) const
{
    glm::vec2 scaledCenter = Center * size;
	
    // TODO: utility function for rotation
    float cosTheta = std::cos(radians);
    float sinTheta = std::sin(radians);
    glm::vec2 rotatedCenter = {
        scaledCenter.x * cosTheta - scaledCenter.y * sinTheta,
        scaledCenter.x * sinTheta + scaledCenter.y * cosTheta
    };
    glm::vec2 worldCenter = rotatedCenter + position;

	// uniform scaling by using max size component
    float worldRadius = Radius * std::max(size.x, size.y);

    return Circle(worldCenter, worldRadius);
}

AABB Circle::getBoundingBox() const
{
	return AABB(
		Center - glm::vec2(Radius),
		Center + glm::vec2(Radius)
	);
}

bool Circle::intersects(const AABB& other) const
{
	return CollisionUtilities::intersects(*this, other);
}

bool Circle::intersects(const Collider& other) const
{
	return other.intersects(*this);
}

bool Circle::intersects(const OBB& other) const
{
	return CollisionUtilities::intersects(*this, other);
}

bool Circle::intersects(const Circle& other) const
{
    // Check if distance between centers is less than sum of radii
    // This is done with product to avoid sqrt (performance)
    glm::vec2 delta = Center - other.Center;
    float rSum = Radius + other.Radius;
    return glm::dot(delta, delta) <= rSum * rSum;
}

} // TileBite