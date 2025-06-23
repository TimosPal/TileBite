#include "utilities/Bitset.hpp"

namespace Engine {

Bitset::Bitset(size_t numBits, bool filled)
	: m_bitsSize(numBits),
	m_words((numBits + BITS_PER_WORD - 1) / BITS_PER_WORD, (filled) ? ~0 : 0)
{}

void Bitset::set(size_t index)
{
	ASSERT(index < m_bitsSize, "Out of range");
	m_words[index / BITS_PER_WORD] |= (WordType(1) << (index % BITS_PER_WORD));
}

void Bitset::clear(size_t index)
{
	ASSERT(index < m_bitsSize, "Out of range");
	m_words[index / BITS_PER_WORD] &= ~(WordType(1) << (index % BITS_PER_WORD));
}

void Bitset::clear()
{
	std::fill(m_words.begin(), m_words.end(), 0);
}

std::vector<size_t> Bitset::getSetBits() const
{
	std::vector<size_t> setBits;
	for (size_t wordIdx = 0; wordIdx < m_words.size(); wordIdx++) {
		WordType current = m_words[wordIdx];
		while (current != 0)
		{
			size_t bit = std::countr_zero(current);
			setBits.push_back(wordIdx * BITS_PER_WORD + bit);
			current &= (current - 1);
		}
	}
	return setBits;
}

size_t Bitset::findLsbIndex() const
{
	for (size_t wordIdx = 0; wordIdx < m_words.size(); wordIdx++)
	{
		WordType word = m_words[wordIdx];
		if (word != 0)
		{
			size_t bit = std::countr_zero(word);
			return wordIdx * BITS_PER_WORD + bit;
		}
	}
	return -1;
}

size_t Bitset::popCount() const
{
	size_t count = 0;
	for (WordType word : m_words)
	{
		count += std::popcount(word);
	}
	return count;
}

Bitset Bitset::operator&(const Bitset& other) const
{
	size_t minSize = std::min(m_bitsSize, other.m_bitsSize);
	Bitset result(minSize);
	size_t wordCount = (minSize + BITS_PER_WORD - 1) / BITS_PER_WORD;
	for (size_t i = 0; i < wordCount; ++i)
		result.m_words[i] = m_words[i] & other.m_words[i];
	return result;
}

Bitset& Bitset::operator&=(const Bitset& other)
{
	size_t minSize = std::min(m_bitsSize, other.m_bitsSize);
	m_bitsSize = minSize;

	size_t wordCount = (minSize + BITS_PER_WORD - 1) / BITS_PER_WORD;

	for (size_t i = 0; i < wordCount; i++)
	{
		m_words[i] &= other.m_words[i];
	}

	for (size_t i = wordCount; i < m_words.size(); i++)
	{
		m_words[i] = 0;
	}

	return *this;
}

Bitset Bitset::operator|(const Bitset& other) const
{
	size_t maxSize = std::max(m_bitsSize, other.m_bitsSize);
	Bitset result(maxSize);
	size_t wordCount = result.m_words.size();

	for (size_t i = 0; i < wordCount; ++i)
	{
		WordType lhs = (i < m_words.size()) ? m_words[i] : 0;
		WordType rhs = (i < other.m_words.size()) ? other.m_words[i] : 0;
		result.m_words[i] = lhs | rhs;
	}
	return result;
}

Bitset& Bitset::operator|=(const Bitset& other)
{
	size_t maxSize = std::max(m_bitsSize, other.m_bitsSize);
	if (m_bitsSize < maxSize)
	{
		m_words.resize((maxSize + BITS_PER_WORD - 1) / BITS_PER_WORD, 0);
		m_bitsSize = maxSize;
	}

	for (size_t i = 0; i < other.m_words.size(); ++i)
	{
		m_words[i] |= other.m_words[i];
	}

	return *this;
}

std::string Bitset::toString() const
{
	std::string str(m_bitsSize, '0');
	for (size_t i = 0; i < m_bitsSize; ++i)
	{
		if (m_words[i / BITS_PER_WORD] & (WordType(1) << (i % BITS_PER_WORD)))
		{
			str[m_bitsSize - 1 - i] = '1'; // MSB on left
		}
	}
	return str;
}

bool Bitset::operator==(const Bitset& other) const
{
	if (m_bitsSize != other.m_bitsSize)
	{
		return false;  // Sizes must be equal
	}

	for (size_t i = 0; i < m_words.size(); i++)
	{
		if (m_words[i] != other.m_words[i])
		{
			return false;  // Word-by-word comparison
		}
	}

	return true;
}

} // Engine 