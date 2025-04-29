#ifndef BITSET_HPP
#define BITSET_HPP

#include "core/pch.hpp"
#include "utilities/misc.hpp"
#include "utilities/assertions.hpp"

namespace Engine {

class Bitset {
public:
	// Friend declaration for std::hash<Engine::Signature<N>>
	friend struct std::hash<Bitset>;

	using WordType = uint32_t;
	static constexpr size_t BITS_PER_WORD = sizeof(WordType) * 8;

	explicit Bitset(size_t numBits);
	Bitset operator&(const Bitset& other) const;
	Bitset operator|(const Bitset& other) const;
	Bitset& operator|=(const Bitset& other);
	bool operator==(const Bitset& other) const;

	void set(size_t index);
	void clear(size_t index);
	std::vector<size_t> getSetBits() const;
	size_t findLsbIndex() const;
	size_t popCount() const;
	size_t getSize() const { return m_bitsSize; }

	std::string toString() const;
private:
	std::vector<WordType> m_words;
	size_t m_bitsSize;
};

} // Engine

// Hash function for Bitset
namespace std {
template<>
struct hash<Engine::Bitset> {
	size_t operator()(const Engine::Bitset& b) const
	{
		size_t seed = 0;
		for (auto word : b.m_words)
		{
			seed ^= std::hash<Engine::Bitset::WordType>()(word)
				+ 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}
};
} // namespace std

#endif // BITSET_HPP
