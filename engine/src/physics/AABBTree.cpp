#include "physics/AABBTree.hpp"
#include "utilities/assertions.hpp"

#include "core/pch.hpp"

namespace Engine {

void AABBTree::insert(const ColliderInfo& colliderInfo)
{
	uint32_t newNodeIndex = createLeafNode(colliderInfo);
	m_leafNodesIndices[colliderInfo.id] = newNodeIndex;

	if (m_rootIndex == NullIndex)
	{
		m_rootIndex = newNodeIndex;
		return;
	}

	uint32_t bestSiblingIndex = findBestSibbling();
	uint32_t newParentNodeIndex = createParentNode(bestSiblingIndex, newNodeIndex);
	refitParentNodes(newParentNodeIndex);
}

uint32_t AABBTree::findBestSibbling()
{
	return m_rootIndex; // TODO
}

uint32_t AABBTree::createLeafNode(const ColliderInfo& colliderInfo)
{
	uint32_t newNodeIndex = createNode(true);
	Node& newNode = m_nodes[newNodeIndex];
	// TODO: find AABB from generic collider, now we only have AABB so this isnt relevant
	newNode.Bounds = colliderInfo.Collider;
	newNode.Value = colliderInfo;

	return newNodeIndex;
}

void AABBTree::refitParentNodes(uint32_t startingIndex)
{
	uint32_t updateIndex = startingIndex;
	while (updateIndex != NullIndex)
	{
		Node& updateNode = m_nodes[updateIndex];
		updateNode.Bounds = AABB::getUnion(m_nodes[updateNode.LeftIndex].Bounds, m_nodes[updateNode.RightIndex].Bounds);
		updateIndex = updateNode.ParentIndex;
	}
}

uint32_t AABBTree::createParentNode(uint32_t bestSiblingIndex, uint32_t newNodeIndex)
{
	uint32_t newParentNodeIndex = createNode(false);
	Node& newParentNode = m_nodes[newParentNodeIndex];
	Node& bestSiblingNode = m_nodes[bestSiblingIndex];

	newParentNode.ParentIndex = bestSiblingNode.ParentIndex;
	newParentNode.LeftIndex = bestSiblingIndex;
	newParentNode.RightIndex = newNodeIndex;

	Node& newNode = m_nodes[newNodeIndex];
	newNode.ParentIndex = newParentNodeIndex;
	bestSiblingNode.ParentIndex = newParentNodeIndex;

	if (m_rootIndex == bestSiblingIndex)
	{
		// Update root if necessary
		m_rootIndex = newParentNodeIndex;
	}
	else
	{
		Node& oldParent = m_nodes[bestSiblingNode.ParentIndex];
		if (oldParent.LeftIndex == bestSiblingIndex)
			oldParent.LeftIndex = newParentNodeIndex;
		else
			oldParent.RightIndex = newParentNodeIndex;
	}

	return newParentNodeIndex;
}

uint32_t AABBTree::createNode(bool isLeaf)
{
	// TODO: pick better strategy for cache locality, now we just push back to the end
	uint32_t newNodeIndex = static_cast<uint32_t>(m_nodes.size());
	Node newNode;
	newNode.IsLeaf = isLeaf;

	m_nodes.push_back(newNode);

	return newNodeIndex;
}

void AABBTree::remove(ID id)
{
	// TODO:
}

bool AABBTree::update(const ColliderInfo& colliderInfo)
{
	auto it = m_leafNodesIndices.find(colliderInfo.id);
	if (it == m_leafNodesIndices.end()) return false;

	uint32_t index = it->second;
	Node& node = m_nodes[index];
	node.Bounds = colliderInfo.Collider;
	node.Value = colliderInfo;

	refitParentNodes(node.ParentIndex);

	return true;
}

std::vector<ID> AABBTree::query(const AABB& collider) const
{
	std::vector<uint32_t> nodeStack;
	std::vector<ID> results;
	uint32_t index = m_rootIndex;

	if (index != NullIndex)
		nodeStack.push_back(index);
	while (!nodeStack.empty())
	{
		index = nodeStack.back();
		nodeStack.pop_back();
		const Node& currNode = m_nodes[index];

		if (!currNode.Bounds.intersects(collider))
			continue; // skip non-overlapping branches

		if (currNode.IsLeaf)
		{
			ASSERT(currNode.Value.has_value(), "Leaf node without value");
			// If the collider intersects, add it to results (Collider may not be AABB)
			const ColliderInfo& info = currNode.Value.value();
			if(info.Collider.intersects(collider))
				results.push_back(info.id);
		}
		else
		{
			// Traverse children
			if (currNode.RightIndex != NullIndex)
				nodeStack.push_back(currNode.RightIndex);
			if (currNode.LeftIndex != NullIndex)
				nodeStack.push_back(currNode.LeftIndex);
		}
	}

	return results;
}

float AABBTree::computeNodeCost(uint32_t index) const
{
	float cost = 0.0f;
	return 0;
}

std::vector<AABB> AABBTree::getInternalBounds() const
{
	std::vector<AABB> bounds;
	for (const auto& node : m_nodes)
	{
		if (node.IsLeaf)
		{
			ASSERT(node.Value.has_value(), "Lead node without value");
			bounds.push_back(node.Value.value().Collider);
		}
		else
		{
			bounds.push_back(node.Bounds);
		}
	}

	return bounds;
}

std::vector<AABB> AABBTree::getLeafColliders() const
{
	std::vector<AABB> colliders;
	for (auto& pair : m_leafNodesIndices)
	{
		uint32_t index = pair.second;
		const Node& node = m_nodes[index];
		colliders.push_back(node.Value.value().Collider);
	}

	return colliders;
}

} // Engine