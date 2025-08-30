#include "physics/CollisionUtilities.hpp"
#include "utilities/Logger.hpp"

namespace TileBite {
namespace CollisionUtilities {

bool intersects(const AABB& a, const OBB& b)
{
	return SATTest(a, b.getCorners());
}
bool intersects(const OBB& a, const AABB& b) { return intersects(b, a); }

bool intersects(const Circle& a, const AABB& b)
{
	LOG_UNIMPLEMENTED;
	return false; // TODO
}
bool intersects(const AABB& a, const Circle& b) { return intersects(b, a); }

bool intersects(const Circle& a, const OBB& b)
{
	LOG_UNIMPLEMENTED;
	return false; // TODO
}
bool intersects(const OBB& a, const Circle& b) { return intersects(b, a); }

bool intersects(const Collider& a, const Circle& b)
{
	return a.intersects(b);
}
bool intersects(const Circle& a, const Collider& b) { return intersects(b, a); }

bool intersects(const Collider& a, const OBB& b)
{
	return a.intersects(b);
}
bool intersects(const OBB& a, const Collider& b) { return intersects(b, a); }

bool intersects(const Collider& a, const AABB& b) 
{
	return a.intersects(b);
}
bool intersects(const AABB& a, const Collider& b) { return intersects(b, a); }

// ==========================================

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

} // CollisionUtilities
} // TileBite