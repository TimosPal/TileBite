#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "utilities/Logger.hpp"

namespace TileBite {

struct AABB; // Forward declaration to avoid circular dependency
struct Collider;
struct OBB;

struct Circle {
    glm::vec2 Center;
    float Radius;

    Circle() : Center(glm::vec2(0.0f)), Radius(1.0f) {}
    Circle(const glm::vec2& center, float radius)
        : Center(center), Radius(radius)
    {}

    Circle toWorldSpace(glm::vec2 position, glm::vec2 size, float radians) const;

    inline bool isValid() const noexcept {
        return Radius >= 0.0f;
    }

    inline float getArea() const {
		return glm::pi<float>() * Radius * Radius;
    }

    AABB getBoundingBox() const;

    bool intersects(const AABB& other) const;
    bool intersects(const Collider& other) const;
    bool intersects(const OBB& other) const;
    bool intersects(const Circle& other) const;

};

} // TileBite

#endif // !CIRCLE_HPP