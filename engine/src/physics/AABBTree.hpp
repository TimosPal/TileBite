#ifndef AABB_TREE_HPP
#define AABB_TREE_HPP

#include "physics/CollisionData.hpp"
#include "physics/Collider.hpp"
#include "physics/Ray2D.hpp"
#include "core/types.hpp"

namespace Engine {

struct ColliderInfo : public Collider {
	ID id;

	ColliderInfo(ID id, const AABB& aabb) : id(id), Collider(aabb) {}
	ColliderInfo(ID id, const OBB& obb) : id(id), Collider(obb) {}
};

// https://box2d.org/files/ErinCatto_DynamicBVH_GDC2019.pdf
class AABBTree {
public:
	void insert(const ColliderInfo& colliderInfo);
	bool remove(ID id);
	bool update(const ColliderInfo& colliderInfo);
	std::vector<CollisionData> query(const AABB& collider, ID excludeID = INVALID_ID) const;
	std::vector<RayHitData> raycastAll(const Ray2D& ray) const;
	std::optional<RayHitData> raycastClosest(const Ray2D& ray) const;

	std::vector<AABB> getInternalBounds() const;
	std::vector<Collider> getLeafColliders() const;
private:
	constexpr static uint32_t NullIndex = UINT32_MAX;

	struct Node {
		uint32_t ParentIndex = NullIndex;
		uint32_t LeftIndex = NullIndex;
		uint32_t RightIndex = NullIndex;

		AABB Bounds;
		bool IsLeaf = false;
		std::optional<ColliderInfo> Value;
	};

	std::vector<Node> m_nodes;
	std::vector<uint32_t> m_freeIndices;
	std::unordered_map<ID, uint32_t> m_leafNodesIndices;
	uint32_t m_rootIndex = NullIndex;

	float computeRefitCostDelta(uint32_t startingIndex, float newParentArea, float bestCost) const;

	uint32_t createNode(bool isLeaf);
	void refitParentNodes(uint32_t startingIndex);
	uint32_t createParentNode(uint32_t bestSiblingIndex, uint32_t newNodeIndex);
	uint32_t findBestSibbling(uint32_t newLeafIndex);
	uint32_t createLeafNode(const ColliderInfo& colliderInfo);
};

} // Engine

#endif // !AABB_TREE_HPP