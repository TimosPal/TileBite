#ifndef AABB_HPP
#define AABB_HPP

#include <glm/glm.hpp>

namespace Engine {

struct AABB {
	glm::vec2 Min; // Minimum point (bottom-left corner)
	glm::vec2 Max; // Maximum point (top-right corner)
	
	AABB() : Min(glm::vec2(0.0f)), Max(glm::vec2(0.0f)) {}
	AABB(const glm::vec2& min, const glm::vec2& max) : Min(min), Max(max) {}
	
	bool contains(const glm::vec2& point) const
	{
		return (point.x >= Min.x && point.x <= Max.x &&
				point.y >= Min.y && point.y <= Max.y);
	}
	
	bool intersects(const AABB& other) const
	{
		return (Min.x <= other.Max.x && Max.x >= other.Min.x &&
				Min.y <= other.Max.y && Max.y >= other.Min.y);
	}

	void setSize(const glm::vec2& min, const glm::vec2& max)
	{
		Min = min;
		Max = max;
	}

	static AABB getUnion(const AABB& a, const AABB& b)
	{
		return AABB(
			glm::min(a.Min, b.Min),
			glm::max(a.Max, b.Max)
		);
	}
};

} // Engine

#endif // !AABB_HPP