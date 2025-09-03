#include "physics/AABBTree.hpp"

#include "core/pch.hpp"

namespace TileBite {

void AABBTree::insert(const ColliderInfo& colliderInfo)
{
	// TODO: tree rebalancing !!!!!!!!!

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
	// Branch and bound algorithm to find the best sibling node for the new leaf

	float bestCost = std::numeric_limits<float>::max();
	uint32_t bestIndex = m_rootIndex;

	static std::vector<uint32_t> nodeQueue;
	nodeQueue.reserve(256);
	nodeQueue.clear();
	uint32_t queueHead = 0;
	nodeQueue.push_back(m_rootIndex);

	Node& newNode = m_nodePool.get(newLeafIndex);
	float newNodeArea = newNode.Bounds.getArea();
	while (queueHead < nodeQueue.size())
	{
		uint32_t currIndex = nodeQueue[queueHead++];
		if(currIndex == NullIndex)
			continue;

		const Node& currNode = m_nodePool.get(currIndex);
		
		float newParentArea = AABB::getUnion(currNode.Bounds, newNode.Bounds).getArea();
		float deltaCost = computeRefitCostDelta(currNode.ParentIndex, newParentArea, bestCost);

		float lowerBound = newNodeArea + deltaCost;
		float cost = newParentArea + deltaCost;
		if (cost < bestCost)
		{
			bestCost = cost;
			bestIndex = currIndex;
		}

		if (lowerBound < bestCost)
		{
			nodeQueue.push_back(currNode.LeftIndex);
			nodeQueue.push_back(currNode.RightIndex);
		}
	}

	ASSERT(bestIndex != NullIndex, "Best sibling index should not be null");
	return bestIndex;
}

uint32_t AABBTree::createLeafNode(const ColliderInfo& colliderInfo)
{
	uint32_t newNodeIndex = m_nodePool.createNode(Node{ .IsLeaf = true });
	Node& newNode = m_nodePool.get(newNodeIndex);
	newNode.Bounds = AABB::inflate(colliderInfo.getAABBBounds());
	newNode.Value = colliderInfo;

	return newNodeIndex;
}

void AABBTree::refitParentNodes(uint32_t startingIndex)
{
	uint32_t updateIndex = startingIndex;
	while (updateIndex != NullIndex)
	{
		Node& updateNode = m_nodePool.get(updateIndex);
		ASSERT(updateNode.IsLeaf == false, "Refitting parent node that is a leaf");
		updateNode.Bounds = AABB::getUnion(m_nodePool.get(updateNode.LeftIndex).Bounds, m_nodePool.get(updateNode.RightIndex).Bounds);
		updateIndex = updateNode.ParentIndex;
	}
}

float AABBTree::computeRefitCostDelta(uint32_t startingIndex, float newParentArea, float bestCost) const
{
	float deltaCost = 0.0f;

	uint32_t currIndex = startingIndex;
	while (currIndex != NullIndex)
	{
		if (deltaCost + newParentArea > bestCost) break;

		const Node& currNode = m_nodePool.get(currIndex);
		ASSERT(currNode.IsLeaf == false, "Refiting cost delta should start with a parent node");
		float oldSA = currNode.Bounds.getArea(); // TODO: Could be stored.
		float newSA = AABB::getUnion(m_nodePool.get(currNode.LeftIndex).Bounds, m_nodePool.get(currNode.RightIndex).Bounds).getArea();
		float delta = newSA - oldSA;
		if (delta <= 0.001) break; // If delta is almost zero then parent delta will not change significantly, so we can stop here
		ASSERT(delta >= 0, "Delta cost should not be negative");
		deltaCost += delta;
		currIndex = currNode.ParentIndex;
	}

	return deltaCost;
}

uint32_t AABBTree::createParentNode(uint32_t bestSiblingIndex, uint32_t newNodeIndex)
{
	uint32_t newParentNodeIndex = m_nodePool.createNode(Node{ .IsLeaf = false });

	Node& newParentNode = m_nodePool.get(newParentNodeIndex);
	Node& bestSiblingNode = m_nodePool.get(bestSiblingIndex);
	Node& newNode = m_nodePool.get(newNodeIndex);

	uint32_t siblingParentIndex = bestSiblingNode.ParentIndex;

	if(siblingParentIndex != NullIndex)
	{
		Node& siblingParentNode = m_nodePool.get(siblingParentIndex);
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

bool AABBTree::remove(ID id)
{
	// TODO: removing nodes leaves a gap in the nodes list, which is not ideal for cache locality
	
	if (m_leafNodesIndices.find(id) == m_leafNodesIndices.end())
		return false;

	uint32_t nodeIndex = m_leafNodesIndices[id]; // Find the index of the leaf node
	Node& node = m_nodePool.get(nodeIndex);
	ASSERT(node.IsLeaf, "Trying to remove non leaf node");

	m_leafNodesIndices.erase(id);
	m_nodePool.freeNode(nodeIndex);

	// If the node is the root, we need to update the root index
	if (nodeIndex == m_rootIndex)
	{
		m_rootIndex = NullIndex; 
		return true;
	}

	// If the node is not the root, we need to remove it from its parent
	uint32_t parentIndex = node.ParentIndex;
	Node& parentNode = m_nodePool.get(parentIndex);
	uint32_t grandParentIndex = parentNode.ParentIndex;

	m_nodePool.freeNode(parentIndex);

	uint32_t siblingIndex = (parentNode.LeftIndex == nodeIndex) ? parentNode.RightIndex : parentNode.LeftIndex;
	Node& siblingNode = m_nodePool.get(siblingIndex);
	// Update the sibling's parent index to point to the grandparent
	siblingNode.ParentIndex = grandParentIndex;

	if (grandParentIndex == NullIndex)
	{
		m_rootIndex = siblingIndex;
	}
	else
	{
		Node& grandParentNode = m_nodePool.get(grandParentIndex);
		if (grandParentNode.LeftIndex == parentIndex)
			grandParentNode.LeftIndex = siblingIndex;
		else
			grandParentNode.RightIndex = siblingIndex;

	}

	refitParentNodes(grandParentIndex);
	return true;
}

bool AABBTree::update(const ColliderInfo& colliderInfo)
{
	auto it = m_leafNodesIndices.find(colliderInfo.id);
	if (it == m_leafNodesIndices.end()) return false;

	Node& node = m_nodePool.get(it->second);
	const float shrinkThreshold = 0.5f;
	if (node.Bounds.contains(colliderInfo) && colliderInfo.getArea() / node.Bounds.getArea() > shrinkThreshold)
	{
		// No need to update if the collider is still within the bounds
		node.Value = colliderInfo;
		return true;
	}

	remove(colliderInfo.id);
	insert(colliderInfo);

	return true;
}

std::vector<CollisionData> AABBTree::query(const Collider& collider, ID excludeID) const {
	switch (collider.Type) {
	case Collider::ColliderType::AABB:
		return query(collider.AABBCollider, excludeID);
	case Collider::ColliderType::OBB:
		return query(collider.OBBCollider, excludeID);
	default:
		ASSERT_FALSE("Unknown collider type");
	}
	return {};
}

std::vector<RayHitData> AABBTree::raycastAll(const Ray2D& ray, ID excludeID) const
{
	static std::vector<uint32_t> nodeStack;
	nodeStack.clear();
	nodeStack.reserve(256);
	std::vector<RayHitData> results;
	uint32_t index = m_rootIndex;

	if (index != NullIndex)
		nodeStack.push_back(index);
	while (!nodeStack.empty())
	{
		index = nodeStack.back();
		nodeStack.pop_back();
		const Node& currNode = m_nodePool.get(index);

		float tmin, tmax;
		if (!ray.intersect(currNode.Bounds, tmin, tmax) || ray.getMaxT() < tmin)
			continue; // skip non-overlapping branches

		if (currNode.IsLeaf)
		{
			ASSERT(currNode.Value.has_value(), "Leaf node without value");
			// If the collider intersects, add it to results (Collider may not be AABB)
			const ColliderInfo& info = currNode.Value.value();
			bool intersects = ray.intersect(info, tmin, tmax);
			if (info.id != excludeID &&
				intersects &&
				ray.getMaxT() >= tmin)
				results.push_back(RayHitData(GenericCollisionData(info.id, info), tmin, tmax));
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

std::optional<RayHitData> AABBTree::raycastClosest(const Ray2D& ray, ID excludeID) const
{
	if (m_rootIndex == NullIndex) return std::nullopt;

	std::vector<uint32_t> stack;
	stack.reserve(256);
	stack.push_back(m_rootIndex);

	float bestT = std::numeric_limits<float>::max();
	std::optional<RayHitData> closestHit;

	while (!stack.empty())
	{
		const Node& node = m_nodePool.get(stack.back());
		stack.pop_back();

		if (node.IsLeaf)
		{
			const auto& info = node.Value.value();
			ASSERT(info.isValid(), "Leaf node without valid collider");

			float tmin, tmax;
			// New tmin is new cloest hist, and not bigger than maxT of the ray
			if (info.id != excludeID &&
				ray.intersect(info, tmin, tmax) &&
				tmin <= ray.getMaxT() &&
				tmin <= bestT)
			{
				bestT = tmin;
				closestHit = RayHitData(GenericCollisionData(info.id, info), tmin, tmax);
			}
		}
		else
		{
			float tminL = std::numeric_limits<float>::max(), tmaxL;
			float tminR = std::numeric_limits<float>::max(), tmaxR;

			bool hitL = node.LeftIndex != NullIndex &&
				ray.intersect(m_nodePool.get(node.LeftIndex).Bounds, tminL, tmaxL) &&
				tminL <= ray.getMaxT() &&
				tminL <= bestT;

			bool hitR = node.RightIndex != NullIndex &&
				ray.intersect(m_nodePool.get(node.RightIndex).Bounds, tminR, tmaxR) &&
				tminR <= ray.getMaxT() &&
				tminR <= bestT;

			// Push closer child last so it's popped first
			if (hitL && hitR)
			{
				if (tminL < tminR)
				{
					stack.push_back(node.RightIndex);
					stack.push_back(node.LeftIndex);
				}
				else
				{
					stack.push_back(node.LeftIndex);
					stack.push_back(node.RightIndex);
				}
			}
			else if (hitL) stack.push_back(node.LeftIndex);
			else if (hitR) stack.push_back(node.RightIndex);
		}
	}

	return closestHit;
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
		const Node& currNode = m_nodePool.get(index);

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

std::vector<Collider> AABBTree::getLeafColliders() const
{
	std::vector<Collider> colliders;
	for (auto& pair : m_leafNodesIndices)
	{
		uint32_t index = pair.second;
		const Node& node = m_nodePool.get(index);
		colliders.push_back(node.Value.value());
	}

	return colliders;
}

} // TileBite