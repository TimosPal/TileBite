#include "scenes/SceneGraph.hpp"

#include "utilities/assertions.hpp"

namespace TileBite {

void SceneGraph::traverseHierarchy(uint32_t previousIndex, uint32_t currentIndex)
{
	Node& currentNode = m_nodePool.get(currentIndex);
	TransformComponent* currentTransform = m_activeWorld.getComponent<TransformComponent>(currentNode.entityID);
	ASSERT(currentTransform, "Linking to non transform components not allowed");

	if (previousIndex != NodePool<Node>::NullIndex)
	{
		Node& parentNode = m_nodePool.get(previousIndex);
		if (parentNode.WorldTransform.isDirty() || currentTransform->isDirty())
		{
			currentNode.WorldTransform = compose(parentNode.WorldTransform, *currentTransform);
		}
	}
	else
	{
		// Root node
		if (currentTransform->isDirty())
		{
			currentNode.WorldTransform = *currentTransform;
		}
	}

	uint32_t childIndex = currentNode.firstChildIndex;
	while (childIndex != NodePool<Node>::NullIndex)
	{
		traverseHierarchy(currentIndex, childIndex);
		childIndex = m_nodePool.get(childIndex).nextSiblingIndex;
	}
}

void SceneGraph::updateWorldTransforms()
{
	// TODO: could optimize by running only on dirty nodes and their children
	for (uint32_t root : m_rootNodes)
	{
		traverseHierarchy(NodePool<Node>::NullIndex, root);
	}
}

TransformComponent& SceneGraph::getWorldTransform(ID entityID)
{
	// Returns either world trasnfrom from graph or from ecs world if not already in graph
	if (m_entityNodeMap.contains(entityID))
	{
		uint32_t index = m_entityNodeMap[entityID];
		return m_nodePool.get(index).WorldTransform;
	}
	else
	{
		TransformComponent* tr = m_activeWorld.getComponent<TransformComponent>(entityID);
		ASSERT(tr, "Entity has no transform component");
		return *tr;
	}
}

void SceneGraph::attachToParent(ID parentID, ID childID)
{
	auto [parentIndex, parentExisted] = createOrGetNode(parentID);
	auto [childIndex, childExisted] = createOrGetNode(childID);
	Node& parentNode = m_nodePool.get(parentIndex);
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

bool SceneGraph::detachFromParent(ID childID)
{
	if(!m_entityNodeMap.contains(childID)) return false;

	uint32_t childIndex = m_entityNodeMap[childID];
	Node& childNode = m_nodePool.get(childIndex);

	// This should never happen by design
	ASSERT(childNode.parentIndex != NodePool<Node>::NullIndex, "Trying to detach entity that has no parent");

	unlink(childNode);

	// Check if childNode has children
	if (childNode.firstChildIndex != NodePool<Node>::NullIndex)
	{
		// It becomes a new root
		m_rootNodes.insert(childIndex);
	}
	else
	{
		// If it has no children it will be removed from the graph since we dont keep single nodes
		m_rootNodes.erase(childIndex);
		m_nodePool.freeNode(childIndex);
		m_entityNodeMap.erase(childID);
	}

	return true;
}

std::tuple<uint32_t, bool> SceneGraph::createOrGetNode(ID id)
{
	auto it = m_entityNodeMap.find(id);
	bool exists = it != m_entityNodeMap.end();
	uint32_t index;
	if (!exists)
	{
		// Store original transform from world space within the node, so the local transform re-transformation based on the
		// parent transform can be correctly calculated
		TransformComponent* tr = m_activeWorld.getComponent<TransformComponent>(id);

		index = m_nodePool.createNode(Node{ .entityID = id, .WorldTransform = *tr });
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
	// Child stays the same
}

} // TileBite