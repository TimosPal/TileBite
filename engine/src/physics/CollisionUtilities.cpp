#include "physics/CollisionUtilities.hpp"
#include "utilities/Logger.hpp"

namespace TileBite {
namespace CollisionUtilities {

bool contains(const AABB& a, const OBB& b)
{
	for (auto c : b.getCorners())
	{
		if (!a.contains(c)) return false;
	}
	return true;
}

bool contains(const OBB& a, const AABB& b) {
	float cosR = cos(-a.Rotation);
	float sinR = sin(-a.Rotation);

	AABB obbToABB(-a.Size * 0.5f, a.Size * 0.5f);
	for (auto c : b.getCorners())
	{
		c -= a.Center;
		glm::vec2 rotated(
			c.x * cosR - c.y * sinR,
			c.x * sinR + c.y * cosR
		);

		if (!obbToABB.contains(rotated)) return false;
	}
	return true;
}

bool contains(const Collider& a, const OBB& b)
{
	return a.contains(b);
}

bool contains(const Collider& a, const AABB& b)
{
	return a.contains(b);
}

bool contains(const OBB& a, const Collider& b) {
	switch (b.Type) {
	case Collider::ColliderType::AABBType: return a.contains(b.AABBCollider);
	case Collider::ColliderType::OBBType:  return a.contains(b.OBBCollider);
	default: return false;
	}
}

bool contains(const AABB& a, const Collider& b) {
	switch (b.Type) {
	case Collider::ColliderType::AABBType: return a.contains(b.AABBCollider);
	case Collider::ColliderType::OBBType:  return a.contains(b.OBBCollider);
	default: return false;
	}
}

// =================================================

// TODO: extend for generic shapes too by not using an array
std::tuple<float, float> axisProjectionMinMax(glm::vec2 axis, const std::array<glm::vec2, 4>& points)
{
	// Project points onto given axis
	float min = std::numeric_limits<float>::infinity();
	float max = -std::numeric_limits<float>::infinity();

	for (const glm::vec2& point : points)
	{
		float projection = glm::dot(point, axis);
		if (projection < min) min = projection;
		if (projection > max) max = projection;
	}

	return { min, max };
}

bool axisOverlap(glm::vec2 axis, const std::array<glm::vec2, 4>& points1, const std::array<glm::vec2, 4>& points2)
{
	auto [min1, max1] = axisProjectionMinMax(axis, points1);
	auto [min2, max2] = axisProjectionMinMax(axis, points2);
	return !(max1 < min2 || max2 < min1); // No overlap if one projection is completely before or after the other
}

bool axisOverlapAABBAxisX(const AABB& aabb, const std::array<glm::vec2, 4>& points2)
{
	float min1 = aabb.Min.x, max1 = aabb.Max.x;
	auto [min2, max2] = axisProjectionMinMax(glm::vec2(1, 0), points2);
	return !(max1 < min2 || max2 < min1); // No overlap if one projection is completely before or after the other
}

bool axisOverlapAABBAxisY(const AABB& aabb, const std::array<glm::vec2, 4>& points2)
{
	float min1 = aabb.Min.y, max1 = aabb.Max.y;
	auto [min2, max2] = axisProjectionMinMax(glm::vec2(0, 1), points2);
	return !(max1 < min2 || max2 < min1); // No overlap if one projection is completely before or after the other
}

bool SATTest(const AABB& aabb, const std::array<glm::vec2, 4>& points2)
{
	// Generic SAT between 2 4 point shapes
	glm::vec2 bAxis1 = glm::normalize(points2[1] - points2[0]);
	glm::vec2 bAxis2 = glm::normalize(points2[3] - points2[0]);

	const auto& pointsAABB = aabb.getCorners();

	// Test all axes
	if (!axisOverlapAABBAxisX(aabb, points2)) return false;
	if (!axisOverlapAABBAxisY(aabb, points2)) return false;
	if (!axisOverlap(bAxis1, pointsAABB, points2)) return false;
	if (!axisOverlap(bAxis2, pointsAABB, points2)) return false;

	return true;
}

bool SATTest(const std::array<glm::vec2, 4>& points1, const std::array<glm::vec2, 4>& points2)
{
	// Generic SAT between 2 4 point shapes
	glm::vec2 aAxis1 = glm::normalize(points1[1] - points1[0]);
	glm::vec2 aAxis2 = glm::normalize(points1[3] - points1[0]);
	glm::vec2 bAxis1 = glm::normalize(points2[1] - points2[0]);
	glm::vec2 bAxis2 = glm::normalize(points2[3] - points2[0]);

	// Test all axes
	if (!axisOverlap(aAxis1, points1, points2)) return false;
	if (!axisOverlap(aAxis2, points1, points2)) return false;
	if (!axisOverlap(bAxis1, points1, points2)) return false;
	if (!axisOverlap(bAxis2, points1, points2)) return false;

	return true;
}

bool intersects(const AABB& a, const OBB& b)
{
	return SATTest(a, b.getCorners());
}

bool intersects(const OBB& a, const AABB& b) {
	return intersects(b, a); // reuse symetrical logic
}

bool intersects(const Collider& a, const OBB& b)
{
	return a.intersects(b);
}

bool intersects(const Collider& a, const AABB& b)
{
	return a.intersects(b);
}

bool intersects(const OBB& a, const Collider& b) {
	return intersects(b, a); // reuse symetrical logic
}

bool intersects(const AABB& a, const Collider& b) {
	return intersects(b, a); // reuse symetrical logic
}

} // CollisionUtilities
} // TileBite