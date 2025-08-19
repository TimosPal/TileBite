#ifndef RAY2D_HPP
#define RAY2D_HPP

#include "physics/AABB.hpp"

namespace Engine {

class Ray2D {
public:

    Ray2D(const glm::vec2& origin, const glm::vec2& dir, float maxT = 10)
        : o(origin), d(dir), invD(1.0f / dir), maxT(maxT) {
    }

    glm::vec2 at(float t) const {
        return o + t * d;
    }

    bool intersect(const AABB& b, float& tmin, float& tmax) const {
        // Intersection with slabs in each axis
        glm::vec2 t0 = (b.Min - o) * invD;
        glm::vec2 t1 = (b.Max - o) * invD;
        // t when entering slab
        tmin = std::max(glm::min(t0.x, t1.x), glm::min(t0.y, t1.y));
        // t when exiting slab
        tmax = std::min(glm::max(t0.x, t1.x), glm::max(t0.y, t1.y));
        return tmax >= tmin;
    }

    float getMaxT() const {
        return maxT;
	}

private:
    glm::vec2 o, d, invD;
    float maxT;
};

} // Engine

#endif // RAY2D_HPP