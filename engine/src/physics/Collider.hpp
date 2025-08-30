#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include <glm/glm.hpp>
#include "core/Types.hpp"

#include "physics/AABB.hpp"
#include "physics/OBB.hpp"
#include "physics/Circle.hpp"
#include "physics/CollisionUtilities.hpp"
#include "utilities/assertions.hpp"

namespace TileBite {

struct Collider {
	enum class ColliderType {
		AABB,
		OBB,
        Circle
	} Type;

	union {
		AABB AABBCollider;
		OBB OBBCollider;
        Circle CircleCollider;
	};

    Collider(const AABB& aabb) :     Type(ColliderType::AABB), AABBCollider(aabb) {}
    Collider(const OBB& obb) :       Type(ColliderType::OBB), OBBCollider(obb) {}
    Collider(const Circle& circle) : Type(ColliderType::Circle), CircleCollider(circle) {}

	AABB getAABBBounds() const {
		switch (Type) {
		case ColliderType::AABB:   return AABBCollider.getBoundingBox();
		case ColliderType::OBB:    return OBBCollider.getBoundingBox();
        case ColliderType::Circle: return CircleCollider.getBoundingBox();
        default: ASSERT_FALSE("Unknown type");
		}
        return AABB(); // Return an empty AABB if type is unknown
	}

    float getArea() const {
        switch (Type) {
        case ColliderType::AABB:   return AABBCollider.getArea();
        case ColliderType::OBB:    return OBBCollider.getArea();
        case ColliderType::Circle: return CircleCollider.getArea();
        default: ASSERT_FALSE("Unknown collider type");
        }
        return false;
    }

    bool isValid() const {
        switch (Type) {
        case ColliderType::AABB:    return AABBCollider.isValid();
        case ColliderType::OBB:     return OBBCollider.isValid();
        case ColliderType::Circle:  return CircleCollider.isValid();
        default: ASSERT_FALSE("Unknown collider type");
        }
        return false;
    }

    template<typename T>
    bool intersects(const T& other) const {
        switch (Type) {
        case ColliderType::AABB:    return AABBCollider.intersects(other);
        case ColliderType::OBB:     return OBBCollider.intersects(other);
        case ColliderType::Circle:  return CircleCollider.intersects(other);
        default: ASSERT_FALSE("Unknown collider type");
        }
        return false;
    }

    bool intersects(const Collider& other) const {
        switch (other.Type) {
        case ColliderType::AABB:    return intersects(other.AABBCollider);
        case ColliderType::OBB:     return intersects(other.OBBCollider);
        case ColliderType::Circle:  return intersects(other.CircleCollider);
        default: ASSERT_FALSE("Unknown collider type");
        }
        return false;
    }

};

} // TileBite

#endif // !COLLIDER_HPP