#ifndef QUERY_RESPONSE_HPP
#define QUERY_RESPONSE_HPP

#include "core/pch.hpp"
#include "ecs/World.hpp"
#include "utilities/Bitset.hpp"

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

			auto entityIDs = archetype->getEntityIDs();
			for (int entityIndex = 0; entityIndex < archetype->getEntitiesCount(); ++entityIndex)
			{
				callWithComponents(func, entityIDs[entityIndex], compStorages, compIndices, entityIndex,
					std::index_sequence_for<ComponentTypes...>{});
			}
		}
	}

	template<typename Func, size_t... I>
	inline void callWithComponents(Func& func,
		ID entityID,
		std::vector<ComponentStorage>& compStorages,
		std::array<uint32_t, sizeof...(ComponentTypes)>& compIndices,
		int entityIndex,
		std::index_sequence<I...>
		)
	{
		func(
			entityID,
			static_cast<std::decay_t<ComponentTypes>*>
			(compStorages[compIndices[I]].get(entityIndex))
			...
		);
	}

private:
	std::vector<std::shared_ptr<Archetype>> m_archetypes;
};

} // namespace Engine

#endif // QUERY_RESPONSE_HPP
