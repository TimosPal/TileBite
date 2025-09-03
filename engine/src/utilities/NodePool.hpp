#ifndef NODE_POOL_HPP
#define NODE_POOL_HPP

#include "core/pch.hpp"

namespace TileBite {

// TODO: remove gaps in the nodes list for better cache locality
// TODO: index reordering for better cache locality

template<typename NodeT>
class NodePool {
public:
	constexpr static uint32_t NullIndex = UINT32_MAX;

	NodeT& get(uint32_t index) { return m_nodes[index]; }
	const NodeT& get(uint32_t index) const { return m_nodes[index]; }

	uint32_t createNode(NodeT&& newNode)
	{
		uint32_t newNodeIndex;
		if (!m_freeIndices.empty())
		{
			// Reuse an index from the free indices
			newNodeIndex = m_freeIndices.back();
			m_freeIndices.pop_back();
			m_nodes[newNodeIndex] = std::move(newNode);
			m_validNodes[newNodeIndex] = true;
		}
		else
		{
			newNodeIndex = static_cast<uint32_t>(m_nodes.size());
			m_nodes.emplace_back(newNode);
			m_validNodes.push_back(true);
		}

		return newNodeIndex;
	}

	void freeNode(uint32_t index)
	{
		// Reset node if necessary
		m_freeIndices.push_back(index);
		m_validNodes[index] = false;
	}

	bool isValid(uint32_t index) const
	{
		return index < m_nodes.size() && m_validNodes[index];
	}

private:
	std::vector<NodeT> m_nodes;
	std::vector<bool> m_validNodes; // TODO: can use a bitset for less memory
	std::vector<uint32_t> m_freeIndices;
};

} // TileBite

#endif // NODE_POOL_HPP