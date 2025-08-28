#ifndef OBB_HPP
#define OBB_HPP

#include <glm/glm.hpp>

#include "utilities/Logger.hpp"

namespace TileBite {

struct AABB; // Forward declaration to avoid circular dependency
struct Collider;

struct OBB {
	glm::vec2 Center; // Center of the OBB
	glm::vec2 Size; // Size of the OBB (width and height)
	float Rotation; // Rotation in radians around the center

	OBB() : Center(glm::vec2(0.0f)), Size(glm::vec2(1.0f)), Rotation(0.0f) {}
	OBB(const glm::vec2& center, const glm::vec2& size, float rotation = 0.0f)
		: Center(center), Size(size), Rotation(rotation)
	{}

    OBB toWorldSpace(glm::vec2 position, glm::vec2 size, float radians) const;

    inline bool isValid() const noexcept {
        return Size.x >= 0.0f && Size.y >= 0.0f;
    }

    inline float getArea() const {
        return Size.x * Size.y;
    }

    AABB getBoundingBox() const;

    bool contains(const AABB& other) const;
    bool contains(const Collider& other) const;

    inline bool contains(const OBB& other) const noexcept {
		// Transform other into AABB space of this OBB
		OBB rotatedOBB = OBB(
            other.Center,
            other.Size,
            other.Rotation - Rotation
        );

        AABB aabb = OBB(
            Center,
            Size,
            Rotation - Rotation
        ).getBoundingBox();

		return rotatedOBB.contains(aabb);
    }

    bool intersects(const AABB& other) const;
    bool intersects(const Collider& other) const;
    bool intersects(const OBB& other) const;

    std::array<glm::vec2, 4> getCorners() const;

};

} // TileBite

#endif // !OBB_HPP