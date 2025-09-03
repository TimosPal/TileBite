#include "scenes/SceneGraph.hpp"

#include "utilities/assertions.hpp"

namespace TileBite {

void SceneGraph::attachToParent(ID parentID, ID childID)
{
	auto [parentIndex, parentExisted] = createOrGetNode(parentID);
	Node& parentNode = m_nodePool.get(parentIndex);
	auto [childIndex, childExisted] = createOrGetNode(childID);
	Node& childNode = m_nodePool.get(childIndex);

	// If child is linked to the same parent, skip.
	if (childNode.parentIndex == parentIndex) return;

	// Newly created parent -> new root -> needs to update root list
	if (!parentExisted)
	{
		m_rootNodes.insert(parentIndex);
	}
	// Not newly created child that has no parent -> was old root -> needs to be removed from root list
	if (childExisted && childNode.parentIndex == NodePool<Node>::NullIndex)
	{
		m_rootNodes.erase(childIndex);
	}

	// If child is linked to a parent, we have to firstly unlink
	if (childNode.parentIndex != NodePool<Node>::NullIndex)
	{
		unlink(childNode);
	}

	// Now that the child is free we can link it without messing up old connections
	link(parentNode, parentIndex, childNode, childIndex);
}

void SceneGraph::detachFromParent(ID childID)
{
	ASSERT(m_entityNodeMap.contains(childID), "Trying to detach entity that is not in the scene graph");

	uint32_t childIndex = m_entityNodeMap[childID];
	Node& childNode = m_nodePool.get(childIndex);

	// This should never happen by design
	ASSERT(childNode.parentIndex != NodePool<Node>::NullIndex, "Trying to detach entity that has no parent");

	unlink(childNode);
	// We dont keep nodes without parents in the scene graph
	m_nodePool.freeNode(childIndex);
	m_entityNodeMap.erase(childID);
}

std::tuple<uint32_t, bool> SceneGraph::createOrGetNode(ID id)
{
	auto it = m_entityNodeMap.find(id);
	bool exists = it != m_entityNodeMap.end();
	uint32_t index;
	if (!exists)
	{
		index = m_nodePool.createNode(Node{ .entityID = id });
		m_entityNodeMap[id] = index;
	}
	else
	{
		index = it->second;
	}

	return { index, exists };
}

void SceneGraph::link(Node& parentNode, uint32_t parentIndex, Node& childNode, uint32_t childIndex)
{
	childNode.parentIndex = parentIndex;

	// If parent has other sibling add to the list by pushing first
	if (parentNode.firstChildIndex != NodePool<Node>::NullIndex)
	{
		childNode.nextSiblingIndex = parentNode.firstChildIndex;
		m_nodePool.get(childNode.nextSiblingIndex).previousSiblingIndex = childIndex;
	}
	// first should always point to latest child
	parentNode.firstChildIndex = childIndex;
}

void SceneGraph::unlink(Node& childNode)
{
	// If not the first node, just link left node with right (even if it's Null)
	if (childNode.previousSiblingIndex != NodePool<Node>::NullIndex)
	{
		m_nodePool.get(childNode.previousSiblingIndex).nextSiblingIndex = childNode.nextSiblingIndex;
	}
	// Trying to unlink first child
	else
	{
		Node& oldParent = m_nodePool.get(childNode.parentIndex);
		// If it's the last child then firstChild will become Null
		oldParent.firstChildIndex = childNode.nextSiblingIndex;
	}

	if (childNode.nextSiblingIndex != NodePool<Node>::NullIndex)
	{
		m_nodePool.get(childNode.nextSiblingIndex).previousSiblingIndex = childNode.previousSiblingIndex;
	}

	// Clear old links for safety
	childNode.parentIndex = NodePool<Node>::NullIndex;
	childNode.previousSiblingIndex = NodePool<Node>::NullIndex;
	childNode.nextSiblingIndex = NodePool<Node>::NullIndex;
}

} // TileBite