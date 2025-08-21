#ifndef AABB_HPP
#define AABB_HPP

#include <glm/glm.hpp>

namespace Engine {

class OBB; // Forward declaration to avoid circular dependency

struct AABB {
	glm::vec2 Min; // Minimum point (bottom-left corner)
	glm::vec2 Max; // Maximum point (top-right corner)
	
	AABB() : Min(glm::vec2(0.0f)), Max(glm::vec2(0.0f)) {}
	AABB(const glm::vec2& min, const glm::vec2& max) : Min(min), Max(max) {}
	
	inline bool isValid() const noexcept
	{
		return Min.x < Max.x && Min.y < Max.y;
	}

	inline bool isEmpty() const noexcept
	{
		return Min.x >= Max.x || Min.y >= Max.y;
	}

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

	bool contains(const OBB& other) const;

	inline bool intersects(const AABB& other) const noexcept
	{
		return (Min.x <= other.Max.x && Max.x >= other.Min.x &&
				Min.y <= other.Max.y && Max.y >= other.Min.y);
	}

	bool intersects(const OBB& other) const;

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

	inline static AABB intersectAABB(const AABB& a, const AABB& b)
	{
		float minX = std::max(a.Min.x, b.Min.x);
		float minY = std::max(a.Min.y, b.Min.y);
		float maxX = std::min(a.Max.x, b.Max.x);
		float maxY = std::min(a.Max.y, b.Max.y);

		if (minX < maxX && minY < maxY) {
			return AABB({ minX, minY }, { maxX, maxY }); // intersection exists
		}

		return AABB(); // empty / invalid
	}

};

} // Engine

#endif // !AABB_HPP