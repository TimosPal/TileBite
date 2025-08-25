#ifndef PHYSICS_ENGINE_HPP
#define PHYSICS_ENGINE_HPP

#include "core/pch.hpp"
#include "utilities/Identifiable.hpp"
#include "physics/AABBTree.hpp"
#include "physics/TilemapColliderGroup.hpp"
#include "physics/CollisionData.hpp"
#include "physics/Ray2D.hpp"
#include "physics/Collider.hpp"

namespace TileBite {

// TODO: layers mask, for filtering collisions.
// TODO: add remove for tilemaps

class PhysicsEngine {
public:
	// Return all colliders overlapping with the given one
	std::vector<CollisionData> queryCollisions(const AABB& collider, ID excludeID = INVALID_ID) const;
	std::vector<RayHitData> raycastAll(const Ray2D& ray) const;
	std::optional<RayHitData> raycastClosest(const Ray2D& ray) const;

	// NOTE: updates also used for additions
	void updateTilemapColliderGroup(ID id, TransformComponent* transform, glm::vec2 tilemapSize, glm::vec2 tileSize, Bitset solidTiles);
	void addTilemapColliderGroup(ID id, TransformComponent* transform, glm::vec2 tilemapSize, glm::vec2 tileSize, Bitset solidTiles);
	
	void updateCollider(ID id, const AABB* collider, TransformComponent* transform);
	void updateCollider(ID id, const OBB* collider, TransformComponent* transform);
	void removeCollider(ID id);

	const std::vector<Collider> getCoreTreeColliders() { return m_coreTree.getLeafColliders(); }
	const std::vector<AABB> getCoreTreeInternalBounds() const { return m_coreTree.getInternalBounds(); }
	const std::vector<AABB> getTilemapTreeInternalBounds() const { return m_tilemapColliderTree.getInternalBounds(); }
	const std::vector<Collider> getTilemapTreeColliders() { return m_tilemapColliderTree.getLeafColliders(); }

private:
	AABBTree m_coreTree;

	std::unordered_map<ID, TilemapColliderGroup> m_tilemapColliderGroups;
	AABBTree m_tilemapColliderTree;
};

} // TileBite

#endif // PHYSICS_ENGINE_HPP
