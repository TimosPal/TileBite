#ifndef COLLISION_DATA_HPP
#define COLLISION_DATA_HPP

#include "physics/Collider.hpp"
#include "core/types.hpp"

namespace TileBite {

// Stores data about a collision, including the ID of the collider and its AABB in world space.
struct GenericCollisionData {
	ID id;
	Collider GenericCollider;

    GenericCollisionData(ID id, const Collider& collider)
        : id(id), GenericCollider(collider)
    {}
};

struct TilemapCollisionData : public GenericCollisionData {
	uint32_t XTilemapIndex;
	uint32_t YTilemapIndex;

    TilemapCollisionData(ID id, const AABB& collider, uint32_t xTilemapIndex, uint32_t yTilemapIndex)
        : GenericCollisionData(id, Collider(collider)), XTilemapIndex(xTilemapIndex), YTilemapIndex(yTilemapIndex) 
    {}
};

struct CollisionData {
    enum Type {
        GenericType,
        TilemapType
    } type;

    union {
        GenericCollisionData Generic;
        TilemapCollisionData Tilemap;
    };

    CollisionData(const GenericCollisionData& data)
        : type(GenericType), Generic(data) {
    }

    CollisionData(const TilemapCollisionData& data)
        : type(TilemapType), Tilemap(data) {
    }

    CollisionData() : type(GenericType), Generic(GenericCollisionData(ID(0), AABB())) {} // dummy default
    ~CollisionData() {}
};

struct RayHitData : public CollisionData {
    float tmin;
    float tmax;

    RayHitData(const GenericCollisionData& data, float tmin_, float tmax_)
        : CollisionData(data), tmin(tmin_), tmax(tmax_) {
    }

    RayHitData(const TilemapCollisionData& data, float tmin_, float tmax_)
        : CollisionData(data), tmin(tmin_), tmax(tmax_) {
    }

	RayHitData() : CollisionData(), tmin(0.0f), tmax(0.0f) {} // dummy default
};

} // TileBite

#endif // !COLLISION_DATA_HPP