#ifndef RAY2D_HPP
#define RAY2D_HPP

#include "physics/AABB.hpp"
#include "physics/OBB.hpp"
#include "physics/Collider.hpp"

namespace TileBite {

class Ray2D {
public:
    Ray2D(const glm::vec2& origin, const glm::vec2& dir, float maxT = 10.0f)
        : o(origin), maxT(maxT)
    {
        d = glm::normalize(dir);
        invD = 1.0f / d;
    }

    glm::vec2 at(float t) const {
        return o + t * d;
    }

    bool intersect(const AABB& b, float& tmin, float& tmax) const;
    bool intersect(const OBB& b, float& tmin, float& tmax) const;
    bool intersect(const Collider& other, float& tmin, float& tmax) const;

    float getMaxT() const {
        return maxT;
	}

    glm::vec2 getOrigin() const {
        return o;
    }
    glm::vec2 getDirection() const {
        return d;
    }
    glm::vec2 getInvDirection() const {
        return invD;
	}

private:
    glm::vec2 o, d, invD;
    float maxT;
};

} // TileBite

#endif // RAY2D_HPP