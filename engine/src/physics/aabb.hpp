#ifndef AABB_HPP
#define AABB_HPP

#include <glm/glm.hpp>

namespace Engine {

struct AABB {
	glm::vec2 Min; // Minimum point (bottom-left corner)
	glm::vec2 Max; // Maximum point (top-right corner)
	
	AABB() : Min(glm::vec2(0.0f)), Max(glm::vec2(0.0f)) {}
	AABB(const glm::vec2& min, const glm::vec2& max) : Min(min), Max(max) {}
	
	inline float getWidth() const noexcept
	{
		return Max.x - Min.x;
	}

	inline float getHeight() const noexcept
	{
		return Max.y - Min.y;
	}

	inline bool contains(const glm::vec2& point) const noexcept
	{
		return (point.x >= Min.x && point.x <= Max.x &&
				point.y >= Min.y && point.y <= Max.y);
	}

	inline bool contains(const AABB& other) const noexcept
	{
		return contains(other.Min) && contains(other.Max);
	}
	
	inline bool intersects(const AABB& other) const noexcept
	{
		return (Min.x <= other.Max.x && Max.x >= other.Min.x &&
				Min.y <= other.Max.y && Max.y >= other.Min.y);
	}

	inline void setSize(const glm::vec2& min, const glm::vec2& max) noexcept
	{
		Min = min;
		Max = max;
	}

	inline float getArea() const noexcept
	{
		return (Max.x - Min.x) * (Max.y - Min.y);
	}

	inline static AABB getUnion(const AABB& a, const AABB& b) noexcept
	{
		return AABB(
			glm::min(a.Min, b.Min),
			glm::max(a.Max, b.Max)
		);
	}

	inline static AABB inflate(const AABB& b, float fat = 0.05f) noexcept
	{
		glm::vec2 margin(fat); // TODO: make margin percentage based?
		return AABB(b.Min - margin, b.Max + margin);
	}
};

} // Engine

#endif // !AABB_HPP