#include "ecs/Signature.hpp"

namespace TileBite {

Signature::Signature() : m_bitset(DEFAULT_SIGNATURE_LENGTH), m_count(0), m_typeIDs({}) {};

Signature::Signature(std::vector<ID>& componentIDs, size_t bitsLength)
	: m_bitset(bitsLength), m_typeIDs(componentIDs), m_count(componentIDs.size())
{
	// Assuming IDs are bit positions in the signature.
	// eg: [A = 0, B = 1, D = 2] -> [1011]
	for (ID id : componentIDs)
	{
		ASSERT(id < bitsLength, "Comp ID exceed max amount of components");
		m_bitset.set(id);
	}
	ASSERT(m_bitset.popCount() == m_count, "Mismatch between bitset and count");
}

bool Signature::operator==(const Signature& other) const
{
	return m_bitset == other.m_bitset;
}

Signature Signature::operator+(const Signature& other) const
{
	Signature result = *this;
	result.m_bitset |= other.m_bitset;
	result.m_typeIDs.insert(result.m_typeIDs.end(), other.m_typeIDs.begin(), other.m_typeIDs.end());
	result.m_count = result.m_bitset.popCount(); // Recalculate count
	return result;
}

Bitset Signature::commonBits(const Signature& other) const
{
	return (m_bitset & other.m_bitset);
}

const size_t Signature::getCount() const
{
	return m_count;
}

uint32_t Signature::getIndex(ID componentID) const
{
	// Returns the component ID to signature mapping
	// eg: given mask 
	// 11010
	// 00010 -> 0
	// 01000 -> 1
	// 10000 -> 2
	std::unordered_map<ID, int32_t> m_mapping;
	ASSERT(componentID < m_bitset.getSize(), "Out of range ID");

	auto it = m_mapping.find(componentID);
	if (it != m_mapping.end())
	{
		return it->second;  // Return cached result
	}

	size_t count = 0;
	Bitset tempBitset = m_bitset;
	size_t index;
	while ((index = tempBitset.findLsbIndex()) != componentID && index != -1)
	{
		count++;
		tempBitset.clear(index);
	}

	// Cache the computed result for future lookups
	m_mapping[componentID] = count;
	return count;
}

std::vector<ID>& Signature::getTypeIDs()
{
	return m_typeIDs;
}

std::string Signature::toString() const
{
	return m_bitset.toString();
}

} // TileBite