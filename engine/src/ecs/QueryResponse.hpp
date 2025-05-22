#ifndef QUERY_RESPONSE_HPP
#define QUERY_RESPONSE_HPP

#include "core/pch.hpp"
#include "ecs/ArchetypeIterator.hpp"

namespace Engine {

template <typename ...ComponentTypes>
class QueryResponse {
public:
	QueryResponse(std::vector<std::shared_ptr<Archetype>>&& archetypes)
		: m_archetypes(archetypes)
	{}

	template<typename Func>
	void each(Func&& func)
	{
		for (auto& archetype : m_archetypes)
		{
			auto& compStorages = archetype->getComponents();
			auto& sig = archetype->getSignature();

			std::array<uint32_t, sizeof...(ComponentTypes)> compIndices = {
				sig.getIndex(GET_TYPE_ID(Component, std::decay_t<ComponentTypes>))...
			};

			for (auto idx : compIndices)
				ASSERT(idx < compStorages.size(), "Component index out of bounds");

			for (int entityIndex = 0; entityIndex < archetype->getEntitiesCount(); ++entityIndex)
			{
				callWithComponents(func, compStorages, compIndices, entityIndex,
					std::index_sequence_for<ComponentTypes...>{});
			}
		}
	}

	template<typename Func, size_t... I>
	inline void callWithComponents(Func& func,
		std::vector<ComponentStorage>& compStorages,
		std::array<uint32_t, sizeof...(ComponentTypes)>& compIndices,
		int entityIndex,
		std::index_sequence<I...>)
	{
		func(
			static_cast<std::decay_t<ComponentTypes>*>
			(compStorages[compIndices[I]].get(entityIndex))
			...
		);
	}

	class Iterator {
	public:
		Iterator(std::vector<std::shared_ptr<Archetype>>::iterator archetypesIt,
			std::vector<std::shared_ptr<Archetype>>& archetypes)
			: m_archetypeIt(archetypesIt), m_archetypes(archetypes)
		{
			if (m_archetypeIt != m_archetypes.end())
			{
				m_entityItStart = (*m_archetypeIt)->begin<ComponentTypes...>();
				m_entityItEnd = (*m_archetypeIt)->end<ComponentTypes...>();
			}
		}

		std::tuple<ComponentTypes*...> operator*()
		{
			return *m_entityItStart;
		}

		Iterator& operator++()
		{
			++m_entityItStart;  // Move to the next element in the current archetype
			while (m_entityItStart == m_entityItEnd)
			{
				++m_archetypeIt;  // Move to the next archetype
				if (m_archetypeIt == m_archetypes.end())
				{
					// If we've reached the end of archetypes, stop iterating
					break;
				}
				// Reset entity iterator for the new archetype
				m_entityItStart = (*m_archetypeIt)->begin<ComponentTypes...>();
				m_entityItEnd = (*m_archetypeIt)->end<ComponentTypes...>();
			}
			return *this;
		}

		bool operator!=(const Iterator& other) const
		{
			// NOTE: not 100% correct if entitie iterators are missmatching,
			// for general range loops this is not a problem.
			return m_archetypeIt != other.m_archetypeIt;
		}

	private:
		std::vector<std::shared_ptr<Archetype>>::iterator m_archetypeIt;
		std::vector<std::shared_ptr<Archetype>>& m_archetypes;
		Archetype::Iterator<ComponentTypes...> m_entityItStart;
		Archetype::Iterator<ComponentTypes...> m_entityItEnd;
	};

	Iterator begin()
	{
		return Iterator(m_archetypes.begin(), m_archetypes);
	}

	Iterator end()
	{
		return Iterator(m_archetypes.end(), m_archetypes);
	}

private:
	std::vector<std::shared_ptr<Archetype>> m_archetypes;
};

} // namespace Engine

#endif // QUERY_RESPONSE_HPP
