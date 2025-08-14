#ifndef COLLISION_DATA_HPP
#define COLLISION_DATA_HPP

#include "physics/AABB.hpp"
#include "core/types.hpp"

namespace Engine {

// Stores data about a collision, including the ID of the collider and its AABB in world space.
struct GenericCollisionData {
	ID id;
	AABB Collider;

    GenericCollisionData(ID id, const AABB& collider)
        : id(id), Collider(collider)
    {}
};

struct TilemapCollisionData : public GenericCollisionData {
	uint32_t XTilemapIndex;
	uint32_t YTilemapIndex;

    TilemapCollisionData(ID id, const AABB& collider, uint32_t xTilemapIndex, uint32_t yTilemapIndex)
        : GenericCollisionData(id, collider), XTilemapIndex(xTilemapIndex), YTilemapIndex(yTilemapIndex) 
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

    ~CollisionData() {}
};

} // Engine

#endif // !COLLISION_DATA_HPP