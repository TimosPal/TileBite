#ifndef OBB_HPP
#define OBB_HPP

#include <glm/glm.hpp>

namespace Engine {

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

    inline bool isValid() const noexcept
    {
        // TODO:
        return false;
    }

    AABB getAABB() const;

    bool contains(const AABB& other) const;
    bool contains(const Collider& other) const;

    inline bool contains(const OBB& other) const noexcept {
		// TODO: Implement OBB contains OBB logic
        return false;
    }

    bool intersects(const AABB& other) const;
    bool intersects(const Collider& other) const;

    inline bool intersects(const OBB& other) const noexcept {
        // TODO: Implement OBB intersects OBB logic
        return false;
    }

};

} // Engine

#endif // !OBB_HPP