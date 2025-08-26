#ifndef AABB_TREE_HPP
#define AABB_TREE_HPP

#include "physics/CollisionData.hpp"
#include "physics/Collider.hpp"
#include "physics/Ray2D.hpp"
#include "core/types.hpp"
#include "utilities/assertions.hpp"


namespace TileBite {

struct ColliderInfo : public Collider {
	ID id;

	ColliderInfo(ID id, const AABB& aabb) : id(id), Collider(aabb) {}
	ColliderInfo(ID id, const OBB& obb) : id(id), Collider(obb) {}
};

// https://box2d.org/files/ErinCatto_DynamicBVH_GDC2019.pdf
class AABBTree {
public:
    // Templated internal query for every collider type
    // (Assumes ColliderT is supported by AABB and ColliderInfo)
    template<typename ColliderT>
    std::vector<CollisionData> query(const ColliderT& collider, ID excludeID) const {
        static std::vector<uint32_t> nodeStack;
        nodeStack.clear();
        nodeStack.reserve(256);
        std::vector<CollisionData> results;
        uint32_t index = m_rootIndex;

        if (index != NullIndex)
            nodeStack.push_back(index);

        while (!nodeStack.empty()) {
            index = nodeStack.back();
            nodeStack.pop_back();
            const Node& currNode = m_nodes[index];

            if (!currNode.Bounds.intersects(collider))
                continue;

            if (currNode.IsLeaf) {
                ASSERT(currNode.Value.has_value(), "Leaf node without value");
                const ColliderInfo& info = currNode.Value.value();
                if (info.id != excludeID && info.intersects(collider))
                    results.push_back(CollisionData(GenericCollisionData(info.id, info)));
            }
            else {
                if (currNode.RightIndex != NullIndex)
                    nodeStack.push_back(currNode.RightIndex);
                if (currNode.LeftIndex != NullIndex)
                    nodeStack.push_back(currNode.LeftIndex);
            }
        }

        return results;
    }

    std::vector<CollisionData> query(const Collider& collider, ID excludeID) const;

	void insert(const ColliderInfo& colliderInfo);
	bool remove(ID id);
	bool update(const ColliderInfo& colliderInfo);
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

} // TileBite

#endif // !AABB_TREE_HPP