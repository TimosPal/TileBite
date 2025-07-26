#ifndef AABB_TREE_HPP
#define AABB_TREE_HPP

#include "physics/AABB.hpp"
#include "core/types.hpp"

namespace Engine {

struct ColliderInfo {
	ID id;
	AABB Collider;
};

// https://box2d.org/files/ErinCatto_DynamicBVH_GDC2019.pdf
class AABBTree {
public:
	void insert(const ColliderInfo& colliderInfo);
	void remove(ID id);
	bool update(const ColliderInfo& colliderInfo);
	std::vector<ID> query(const AABB& collider) const;

	std::vector<AABB> getInternalBounds() const;
	std::vector<AABB> getLeafColliders() const;
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
	std::unordered_map<ID, uint32_t> m_leafNodesIndices;
	uint32_t m_rootIndex = NullIndex;

	float computeNodeCost(uint32_t index) const;

	uint32_t createNode(bool isLeaf);
	void refitParentNodes(uint32_t startingIndex);
	uint32_t createParentNode(uint32_t bestSiblingIndex, uint32_t newNodeIndex);
	uint32_t findBestSibbling();
	uint32_t createLeafNode(const ColliderInfo& colliderInfo);
};

} // Engine

#endif // !AABB_TREE_HPP