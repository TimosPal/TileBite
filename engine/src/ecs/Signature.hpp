#ifndef SIGNATURE_HPP
#define SIGNATURE_HPP

#include "core/pch.hpp"
#include "core/Types.hpp"
#include "utilities/Identifiable.hpp"
#include "utilities/Assertions.hpp"

namespace Engine {

template<size_t NUMBER_OF_BITS>
class Signature {
public:
	// Friend declaration for std::hash<Engine::Signature<N>>
	friend struct std::hash<Signature>;

	Signature() = default;

	template <typename... IDs>
	Signature(IDs... componentIDs) 
	{
		ASSERT(((componentIDs < NUMBER_OF_BITS) && ...), "Comp ID exceed max amount of components");

		// Assuming IDs are bit positions in the signature.
		// eg: [A = 0, B = 1, D = 2] -> [1011]
		((m_bitset.set(componentIDs)), ...);
	}

	bool operator==(const Signature& other) const
	{
		return m_bitset == other.m_bitset;
	}

	Signature operator+(const Signature& other) const
	{
		Signature result = *this;
		result.m_bitset |= other.m_bitset;
		return result;
	}

	bool isSubsetOf(const Signature& other) const 
	{
		return (m_bitset & other.m_bitset) == m_bitset;
	}

	bool isSupersetOf(const Signature& other) const 
	{
		return (m_bitset & other.m_bitset) == other.m_bitset;
	}

	// New bits introduced by this signature (superset - subset)
	std::bitset<NUMBER_OF_BITS> newBits(const Signature& other) const
	{
		return (m_bitset & ~other.m_bitset);
	}

	// Bits removed from this signature (subset - superset)
	std::bitset<NUMBER_OF_BITS> removedBits(const Signature& other) const
	{
		return (other.m_bitset & ~m_bitset);
	}

	// Common bits between this signature and another
	std::bitset<NUMBER_OF_BITS> commonBits(const Signature& other) const
	{
		return (m_bitset & other.m_bitset);
	}

	const size_t getCount() const 
	{ 
		return m_bitset.count(); 
	}

	// TODO: Optimization to avoid map: return popCount(mask & (sig(id) - 1))
	// needs custom bitmask implementation to support -1.
	int32_t getIndex(ID componentID) const
	{
		// Returns the component ID to signature mapping
		// eg: given mask 
		// 11010
		// 00010 -> 0
		// 01000 -> 1
		// 10000 -> 2
		static std::unordered_map<ID, int32_t> s_mapping;
		ASSERT(componentID < NUMBER_OF_BITS, "Out of range ID");

		auto it = s_mapping.find(componentID);
		if (it != s_mapping.end())
		{
			return it->second;  // Return cached result
		}

		uint32_t count = 0;
		for (size_t i = 0; i < componentID; i++)
		{
			if (m_bitset.test(i)) count++;
		}

		// Cache the computed result for future lookups
		s_mapping[componentID] = count;
		return count;
	}

	std::string toString() const
	{
		return m_bitset.to_string();
	}

private:
	std::bitset<NUMBER_OF_BITS> m_bitset;
};

constexpr uint32_t MAX_NUMBER_OF_COMPONENTS = 128;
using CompSignature = Signature<MAX_NUMBER_OF_COMPONENTS>;

} // Engine

namespace std {
template<size_t N>
struct hash<Engine::Signature<N>> {
	std::size_t operator()(const Engine::Signature<N>& sig) const 
	{
		return std::hash<std::bitset<N>>{}(sig.m_bitset);
	}
};
}

#endif // !SIGNATURE_HPP