#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include <glm/glm.hpp>
#include "core/Types.hpp"

#include "physics/AABB.hpp"
#include "physics/OBB.hpp"
#include "physics/CollisionUtilities.hpp"

namespace TileBite {

struct Collider {
	enum class ColliderType {
		AABBType,
		OBBType
	} Type;

	union {
		AABB AABBCollider;
		OBB OBBCollider;
	};

    Collider(const AABB& aabb) : Type(ColliderType::AABBType), AABBCollider(aabb) {}
    Collider(const OBB& obb) : Type(ColliderType::OBBType), OBBCollider(obb) {}

	AABB getAABBBounds() const {
		switch (Type) {
		case ColliderType::AABBType:
			return AABBCollider.getBoundingBox();
		case ColliderType::OBBType:
			return OBBCollider.getBoundingBox();
		default:
			return AABB(); // Return an empty AABB if type is unknown
		}
	}

    float getArea() const {
        switch (Type) {
        case ColliderType::AABBType: return AABBCollider.getArea();
        case ColliderType::OBBType:  return OBBCollider.getArea();
        default: return 0.0f;
        }
    }

    bool isValid() const {
        switch (Type) {
        case ColliderType::AABBType: return AABBCollider.isValid();
        case ColliderType::OBBType:  return OBBCollider.isValid();
        default: return false;
        }
    }

    template<typename T>
    bool contains(const T& other) const {
        switch (Type) {
        case ColliderType::AABBType: return AABBCollider.contains(other);
        case ColliderType::OBBType:  return OBBCollider.contains(other);
        default: return false;
        }
    }

    template<typename T>
    bool intersects(const T& other) const {
        switch (Type) {
        case ColliderType::AABBType: return AABBCollider.intersects(other);
        case ColliderType::OBBType:  return OBBCollider.intersects(other);
        default: return false;
        }
    }

    bool contains(const Collider& other) const {
        switch (other.Type) {
        case ColliderType::AABBType: return contains(other.AABBCollider);
        case ColliderType::OBBType:  return contains(other.OBBCollider);
        default: return false;
        }
    }

    bool intersects(const Collider& other) const {
        switch (other.Type) {
        case ColliderType::AABBType: return intersects(other.AABBCollider);
        case ColliderType::OBBType:  return intersects(other.OBBCollider);
        default: return false;
        }
    }

};

} // TileBite

#endif // !COLLIDER_HPP