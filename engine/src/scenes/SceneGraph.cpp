#include "scenes/SceneGraph.hpp"

#include "utilities/assertions.hpp"

namespace TileBite {

void SceneGraph::traverseHierarchy(uint32_t previousIndex, uint32_t currentIndex, World& activeWorld)
{
	// TODO: optimization by early stopping etc

	Node& currentNode = m_nodePool.get(currentIndex);

	// local transform
	TransformComponent* localTrans = activeWorld.getComponent<TransformComponent>(currentNode.entityID);
	glm::vec2 localPos = glm::vec2(0.0f);
	glm::vec2 localSize = glm::vec2(1.0f);
	float localRot = 0.0f;
	if (localTrans)
	{
		localPos = localTrans->getPosition();
		localSize = localTrans->getSize();
		localRot = localTrans->getRotation();
	}

	// Parent world transform
	glm::vec2 parentPos = glm::vec2(0.0f);
	glm::vec2 parentSize = glm::vec2(1.0f);
	float parentRot = 0.0f;
	if (previousIndex != NodePool<Node>::NullIndex)
	{
		Node& parentNode = m_nodePool.get(previousIndex);
		parentPos = parentNode.WorldTransform.getPosition();
		parentSize = parentNode.WorldTransform.getSize();
		parentRot = parentNode.WorldTransform.getRotation();
	}

	// Calculate world transform
	glm::vec2 scaledLocalPos = localPos * parentSize;
    float s = std::sin(parentRot);
    float c = std::cos(parentRot);
    glm::vec2 rotatedLocalPos = {
		scaledLocalPos.x * c - scaledLocalPos.y * s,
		scaledLocalPos.x * s + scaledLocalPos.y * c
    };
    glm::vec2 worldPos = parentPos + rotatedLocalPos;
    float worldRot = parentRot + localRot;
    glm::vec2 worldSize = localSize * parentSize;
    currentNode.WorldTransform.setPosition(worldPos);
    currentNode.WorldTransform.setRotation(worldRot);
    currentNode.WorldTransform.setSize(worldSize);

	uint32_t childIndex = currentNode.firstChildIndex;
	while (childIndex != NodePool<Node>::NullIndex)
	{
		traverseHierarchy(currentIndex, childIndex, activeWorld);

		childIndex = m_nodePool.get(childIndex).nextSiblingIndex;
	}
}

void SceneGraph::updateWorldTransforms(World& activeWorld)
{
	for (uint32_t root : m_rootNodes)
	{
		traverseHierarchy(NodePool<Node>::NullIndex, root, activeWorld);
	}
}

TransformComponent& SceneGraph::getWorldTransform(ID entityID)
{
	ASSERT(m_entityNodeMap.contains(entityID), "Entity not found in scene graph");
	uint32_t index = m_entityNodeMap[entityID];
	return m_nodePool.get(index).WorldTransform;
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

void SceneGraph::detachFromParent(ID childID)
{
	ASSERT(m_entityNodeMap.contains(childID), "Trying to detach entity that is not in the scene graph");

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
	// Child stays the same
}

} // TileBite