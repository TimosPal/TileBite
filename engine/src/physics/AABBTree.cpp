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

	uint32_t bestSiblingIndex = findBestSibbling(newNodeIndex);
	uint32_t newParentNodeIndex = createParentNode(bestSiblingIndex, newNodeIndex);
	refitParentNodes(newParentNodeIndex);
}

uint32_t AABBTree::findBestSibbling(uint32_t newLeafIndex)
{
	const AABB& newBounds = m_nodes[newLeafIndex].Bounds;
	float bestCost = std::numeric_limits<float>::max();
	uint32_t bestIndex = NullIndex;

	for (auto nodeIt : m_leafNodesIndices)
	{
		Node currNode = m_nodes[nodeIt.second];
		if (nodeIt.second == newLeafIndex)
			continue;

		const AABB& candidateBounds = currNode.Bounds;
		AABB merged = AABB::getUnion(candidateBounds, newBounds);
		float cost = merged.getArea();

		if (cost < bestCost)
		{
			bestCost = cost;
			bestIndex = nodeIt.second;
		}
	}

	return bestIndex;
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
		ASSERT(updateNode.IsLeaf == false, "Refitting parent node that is a leaf");
		updateNode.Bounds = AABB::getUnion(m_nodes[updateNode.LeftIndex].Bounds, m_nodes[updateNode.RightIndex].Bounds);
		updateIndex = updateNode.ParentIndex;
	}
}

uint32_t AABBTree::createParentNode(uint32_t bestSiblingIndex, uint32_t newNodeIndex)
{
	uint32_t newParentNodeIndex = createNode(false);

	Node& newParentNode = m_nodes[newParentNodeIndex];
	Node& bestSiblingNode = m_nodes[bestSiblingIndex];
	Node& newNode = m_nodes[newNodeIndex];

	uint32_t siblingParentIndex = bestSiblingNode.ParentIndex;

	if(siblingParentIndex != NullIndex)
	{
		Node& siblingParentNode = m_nodes[siblingParentIndex];
		if (siblingParentNode.LeftIndex == bestSiblingIndex)
			siblingParentNode.LeftIndex = newParentNodeIndex;
		else
			siblingParentNode.RightIndex = newParentNodeIndex;
	}
	else
	{
		// This is the root node, so we update the root index
		m_rootIndex = newParentNodeIndex;
	}

	newParentNode.ParentIndex = siblingParentIndex;
	newParentNode.LeftIndex = bestSiblingIndex;
	newParentNode.RightIndex = newNodeIndex;
	newNode.ParentIndex = newParentNodeIndex;
	bestSiblingNode.ParentIndex = newParentNodeIndex;

	return newParentNodeIndex;
}

uint32_t AABBTree::createNode(bool isLeaf)
{
	// TODO: maybe defragment indices to avoid gaps in the nodes list

	Node newNode = {};
	newNode.IsLeaf = isLeaf;

	uint32_t newNodeIndex;
	if (!m_freeIndices.empty())
	{
		// Reuse an index from the free indices
		newNodeIndex = m_freeIndices.back();
		m_freeIndices.pop_back();
		m_nodes[newNodeIndex] = newNode;
	}
	else
	{
		newNodeIndex = static_cast<uint32_t>(m_nodes.size());
		m_nodes.push_back(newNode);
	}

	return newNodeIndex;
}

void AABBTree::remove(ID id)
{
	// TODO: removing nodes leaves a gap in the nodes list, which is not ideal for cache locality

	ASSERT(m_leafNodesIndices.find(id) != m_leafNodesIndices.end(), "Trying to remove a node that does not exist in the tree");
	uint32_t nodeIndex = m_leafNodesIndices[id]; // Find the index of the leaf node
	Node& node = m_nodes[nodeIndex];
	ASSERT(node.IsLeaf, "Trying to remove non leaf node");

	m_leafNodesIndices.erase(id);
	m_freeIndices.push_back(nodeIndex);

	// If the node is the root, we need to update the root index
	if (nodeIndex == m_rootIndex)
	{
		m_rootIndex = NullIndex; 
		return;
	}

	// If the node is not the root, we need to remove it from its parent
	uint32_t parentIndex = node.ParentIndex;
	Node& parentNode = m_nodes[parentIndex];
	uint32_t grandParentIndex = parentNode.ParentIndex;

	m_freeIndices.push_back(parentIndex);

	uint32_t siblingIndex = (parentNode.LeftIndex == nodeIndex) ? parentNode.RightIndex : parentNode.LeftIndex;
	Node& siblingNode = m_nodes[siblingIndex];
	// Update the sibling's parent index to point to the grandparent
	siblingNode.ParentIndex = grandParentIndex;

	if (grandParentIndex == NullIndex)
	{
		m_rootIndex = siblingIndex;
	}
	else
	{
		Node& grandParentNode = m_nodes[grandParentIndex];
		if (grandParentNode.LeftIndex == parentIndex)
			grandParentNode.LeftIndex = siblingIndex;
		else
			grandParentNode.RightIndex = siblingIndex;

	}

	refitParentNodes(grandParentIndex);
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
	std::vector<uint32_t> nodeStack;
	std::vector<AABB> results;
	uint32_t index = m_rootIndex;

	if (index != NullIndex)
		nodeStack.push_back(index);
	while (!nodeStack.empty())
	{
		index = nodeStack.back();
		nodeStack.pop_back();
		const Node& currNode = m_nodes[index];

		results.push_back(currNode.Bounds);

		if (!currNode.IsLeaf)
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