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
	// Return CollisionData for each overlapping collider with ColliderT
	// (Assumes ColliderT is supported by TilemapColliderGroup and AABBTree)
	template<typename ColliderT>
	std::vector<CollisionData> query(const ColliderT& collider, ID excludeID = INVALID_ID) const
	{
		// TODO: maybe use a different tree for static colliders
		// tilemaps could be simpified to 

		// Need to exclude the ID to avoid self-collision
		auto collisionData = m_coreTree.query(collider, excludeID);
		auto tilemapChunksCollisionData = m_tilemapColliderTree.query(collider, excludeID);

		collisionData.reserve(collisionData.size() + tilemapChunksCollisionData.size());
		for (const CollisionData& tilemapCollisionData : tilemapChunksCollisionData)
		{
			auto it = m_tilemapColliderGroups.find(tilemapCollisionData.Generic.id);
			ASSERT(it != m_tilemapColliderGroups.end(), "Tilemap collider group not found in the map");

			const TilemapColliderGroup& group = it->second;
			auto groupColliderData = group.query(collider);
			collisionData.insert(collisionData.end(), groupColliderData.begin(), groupColliderData.end());
		}

		return collisionData;
	}

	std::vector<RayHitData> raycastAll(const Ray2D& ray, ID excludeID = INVALID_ID) const;
	std::optional<RayHitData> raycastClosest(const Ray2D& ray, ID excludeID = INVALID_ID) const;

	// NOTE: updates also used for additions
	void updateTilemapColliderGroup(ID id, TransformComponent* transform, glm::vec2 tilemapSize, glm::vec2 tileSize, Bitset solidTiles);
	void addTilemapColliderGroup(ID id, TransformComponent* transform, glm::vec2 tilemapSize, glm::vec2 tileSize, Bitset solidTiles);
	
	template<typename ColliderT>
	void updateCollider(ID id, const ColliderT* collider, TransformComponent* transform)
	{
		ColliderT worldSpaceAABB = collider->toWorldSpace(
			transform->getPosition(),
			transform->getSize(),
			transform->getRotation()
		);

		ColliderInfo info(id, worldSpaceAABB);
		bool updated = m_coreTree.update(info);
		if (!updated) m_coreTree.insert(info);
	}

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
