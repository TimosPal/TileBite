#ifndef SIGNATURE_HPP
#define SIGNATURE_HPP

#include "core/pch.hpp"
#include "core/Types.hpp"
#include "utilities/Identifiable.hpp"
#include "utilities/assertions.hpp"
#include "utilities/Bitset.hpp"

namespace Engine {

constexpr uint32_t DEFAULT_SIGNATURE_LENGTH = 128;

class Signature {
public:
	// Friend declaration for std::hash<Engine::Signature<N>>
	friend struct std::hash<Signature>;

	Signature();
	Signature(std::vector<ID>& componentIDs, size_t bitsLength = DEFAULT_SIGNATURE_LENGTH);
	bool operator==(const Signature& other) const;
	Signature operator+(const Signature& other) const;

	Bitset commonBits(const Signature& other) const;
	const size_t getCount() const;
	uint32_t getIndex(ID componentID) const;
	std::vector<ID>& getTypeIDs();
	std::string toString() const;

private:
	Engine::Bitset m_bitset;
	std::vector<ID> m_typeIDs;
	std::unordered_map<ID, int32_t> m_mapping;
	size_t m_count;
};

} // Engine

namespace std {
template<>
struct hash<Engine::Signature> {
	std::size_t operator()(const Engine::Signature& sig) const 
	{
		return std::hash<Engine::Bitset>{}(sig.m_bitset);
	}
};

}

#endif // !SIGNATURE_HPP