#ifndef SCENE_GRAPH_HPP
#define SCENE_GRAPH_HPP

#include "core/pch.hpp"
#include "core/Types.hpp"

#include "utilities/NodePool.hpp"

namespace TileBite {

class SceneGraph {
public:
	SceneGraph() = default;
	~SceneGraph() = default;

	void attachToParent(ID parentID, ID childID);
	void detachFromParent(ID childID);

private:
	struct Node {
		uint32_t parentIndex = NodePool<Node>::NullIndex;
		uint32_t firstChildIndex = NodePool<Node>::NullIndex;
		uint32_t nextSiblingIndex = NodePool<Node>::NullIndex;
		uint32_t previousSiblingIndex = NodePool<Node>::NullIndex;

		ID entityID = INVALID_ID;
	};

	NodePool<Node> m_nodePool;
	std::unordered_map<ID, uint32_t> m_entityNodeMap;
	std::unordered_set<uint32_t> m_rootNodes;

	std::tuple<uint32_t, bool> createOrGetNode(ID id);
	void link(Node& parentNode, uint32_t parentIndex, Node& childNode, uint32_t childIndex);
	void unlink(Node& childNode);

};

} // TileBite

#endif // SCENE_GRAPH_HPP