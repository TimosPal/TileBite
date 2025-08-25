#ifndef BITSET_HPP
#define BITSET_HPP

#include "core/pch.hpp"
#include "utilities/misc.hpp"
#include "utilities/assertions.hpp"

namespace TileBite {

class Bitset {
public:
	// Friend declaration for std::hash<TileBite::Signature<N>>
	friend struct std::hash<Bitset>;

	using WordType = uint32_t;
	static constexpr size_t BITS_PER_WORD = sizeof(WordType) * 8;

	explicit Bitset(size_t numBits = 0, bool filled = false);
	Bitset operator&(const Bitset& other) const;
	Bitset& operator&=(const Bitset& other);
	Bitset operator|(const Bitset& other) const;
	Bitset& operator|=(const Bitset& other);
	bool operator==(const Bitset& other) const;
	bool isSet(size_t index) const;
	void set(size_t index);
	void clear(size_t index);
	void clear();
	std::vector<size_t> getSetBits() const;
	size_t findLsbIndex() const;
	size_t popCount() const;
	size_t getSize() const { return m_bitsSize; }

	std::string toString() const;
private:
	std::vector<WordType> m_words;
	size_t m_bitsSize;
};

} // TileBite

// Hash function for Bitset
namespace std {
template<>
struct hash<TileBite::Bitset> {
	size_t operator()(const TileBite::Bitset& b) const
	{
		size_t seed = 0;
		for (auto word : b.m_words)
		{
			seed ^= std::hash<TileBite::Bitset::WordType>()(word)
				+ 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}
};
} // std

#endif // BITSET_HPP
