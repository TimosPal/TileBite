#ifndef PHYSICS_ENGINE_HPP
#define PHYSICS_ENGINE_HPP

#include "core/pch.hpp"
#include "utilities/Identifiable.hpp"
#include "physics/AABB.hpp"
#include "physics/AABBTree.hpp"
#include "physics/TilemapColliderGroup.hpp"
#include "physics/CollisionData.hpp"
#include "physics/Ray2D.hpp"

namespace Engine {

// TODO: layers mask, for filtering collisions.

class PhysicsEngine {
public:
	// Return all colliders overlapping with the given one
	std::vector<CollisionData> queryCollisions(const AABB& collider, ID excludeID = INVALID_ID) const;
	std::vector<RayHitData> raycastAll(const Ray2D& ray) const;
	std::vector<RayHitData> raycastClosest(const Ray2D& ray) const;

	void addCollider(ID id, AABB* collider, TransformComponent* transform);
	void addTilemapColliderGroup(ID id, TransformComponent* transform, glm::vec2 tilemapSize, glm::vec2 tileSize, Bitset solidTiles);
	void updateTilemapColliderGroup(ID id, TransformComponent* transform, glm::vec2 tilemapSize, glm::vec2 tileSize, Bitset solidTiles);
	void removeCollider(ID id);
	void updateCollider(ID id, AABB* collider, TransformComponent* transform);

	const std::vector<AABB> getCoreTreeColliders() { return m_coreTree.getLeafColliders(); }
	const std::vector<AABB> getCoreTreeInternalBounds() const { return m_coreTree.getInternalBounds(); }
	const std::vector<AABB> getTilemapTreeInternalBounds() const { return m_tilemapColliderTree.getInternalBounds(); }
	const std::vector<AABB> getTilemapTreeColliders() { return m_tilemapColliderTree.getLeafColliders(); }

private:
	AABBTree m_coreTree;

	std::unordered_map<ID, TilemapColliderGroup> m_tilemapColliderGroups;
	AABBTree m_tilemapColliderTree;
};

} // Engine

#endif // PHYSICS_ENGINE_HPP
