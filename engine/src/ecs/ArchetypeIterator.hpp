#ifndef ARCHETYPE_ITERATOR_HPP
#define ARCHETYPE_ITERATOR_HPP

#include "ecs/Archetype.hpp"
#include "utilities/misc.hpp"

namespace Engine {

template <typename ...ComponentTypes>
class Archetype::Iterator {
public:
	Iterator() = default;

	Iterator(Archetype* archetype, size_t index = 0)
		: m_index(index)
	{
		auto& sig = archetype->getSignature();
		std::vector<uint32_t> columIndices = {
			sig.getIndex(GET_TYPE_ID(Component, std::decay_t<ComponentTypes>))
			... 
		};

		m_componentIts.reserve(columIndices.size());
		auto& components = archetype->getComponents();
		for (auto index : columIndices)
		{
			m_componentIts.push_back(components[index].begin());
		}
	}

	std::tuple<ComponentTypes*...> operator*() {
		return makeTupleFromList(m_componentIts);
	}

	Iterator& operator++()
	{
		m_index++;
		for (auto& it : m_componentIts)
		{
			++it;
		}
		return *this;
	}

	bool operator!=(const Iterator& other) const
	{
		return m_index != other.m_index;
	}

	bool operator==(const Iterator& other) const
	{
		return m_index == other.m_index;
	}
private:

	std::tuple<ComponentTypes*...> makeTupleFromList(std::vector<ComponentStorage::Iterator>& rawPtrs)
	{
		return makeTupleHelper(rawPtrs, std::index_sequence_for<ComponentTypes...>{});
	}

	template<std::size_t... Is>
	std::tuple<ComponentTypes*...> makeTupleHelper(std::vector<ComponentStorage::Iterator>& rawPtrs, std::index_sequence<Is...>)
	{
		return std::make_tuple(static_cast<ComponentTypes*>(*rawPtrs[Is])...);
	}

	std::vector<ComponentStorage::Iterator> m_componentIts;
	size_t m_index;
};

} // Engine

#endif // !ARCHETYPE_ITERATOR_HPP
