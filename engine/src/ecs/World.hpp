#ifndef WORLD_HPP
#define WORLD_HPP

#include "core/pch.hpp"
#include "ecs/Archetype.hpp"
#include "ecs/Signature.hpp"
#include "utilities/Identifiable.hpp"
#include "core/Types.hpp"

namespace Engine {

// Can retrieve an entity row from the ComponentStorage given
// an archetype reference.
struct EntityRecord {
	uint32_t entityIndex;
	std::shared_ptr<Archetype> archetype;
};

class World {
public:
	ID createEntity()
	{
		ID entityID = GET_INSTANCE_ID(World, void); // consider global ids under World.
		CompSignature sig;
		auto [it, inserted] = m_archetypes.try_emplace(sig, std::make_shared<Archetype>(Archetype(sig, {})));
		m_entityRecords.emplace(entityID, EntityRecord{0, it->second});
		// NOTE: empty archetype does not need any data storage so index is just a placeholder.

		return entityID;
	}

	template <typename ComponentType>
	ComponentType* getComponent(ID entityID)
	{
		auto entityIt = m_entityRecords.find(entityID);
		ASSERT(entityIt != m_entityRecords.end(), "Entity not found");

		EntityRecord& rec = entityIt->second;
		uint32_t componentIndex = rec.archetype->getSignature().getIndex(GET_TYPE_ID(Component, ComponentType));
		void* comp = rec.archetype->getComponent(rec.entityIndex, componentIndex);
		return reinterpret_cast<ComponentType*>(comp);
	}

	template <typename ...ComponentTypes>
	void addComponents(ID entityID, ComponentTypes&&... components)
	{
		// Ensure that all types are unique
		std::set<ID> uniqueTypes;
		(uniqueTypes.insert(GET_TYPE_ID(Component, std::decay_t<ComponentTypes>)), ...);
		ASSERT(sizeof...(ComponentTypes) == uniqueTypes.size(), "ComponentTypes must be unique");

		auto entityIt = m_entityRecords.find(entityID);
		ASSERT(entityIt != m_entityRecords.end(), "Entity not found");

		EntityRecord& rec = entityIt->second;
		CompSignature& oldSig = rec.archetype->getSignature();
		std::vector<ID> newTypeIDs = { GET_TYPE_ID(Component, std::decay_t<ComponentTypes>)... };
		CompSignature addedSig = CompSignature(newTypeIDs);
		ASSERT(oldSig.commonBits(addedSig) == 0, "A component was already added");
		CompSignature newSig = oldSig + addedSig;

		// Add component sizes to map.
		((m_typeIDSizes[GET_TYPE_ID(Component, std::decay_t<decltype(components)>)] = sizeof(components)), ...);
		
		// Create sizes array for new archetype
		auto createSizes = [&]() {
			std::vector<size_t> result;
			result.reserve(newSig.getTypeIDs().size());
			for (const auto& id : newSig.getTypeIDs())
			{
				auto it = m_typeIDSizes.find(id);
				ASSERT(it != m_typeIDSizes.end(), "Size not found");
				result.push_back(it->second);
			}
			return result;
		};

		// When an entity gets a new components that means its archetype changes so we have to
		// either make a new one if no entities with such type existed, or just retrieve an already existing one
		// and append to its storage the appropriate components.
		auto [archetypeIt, inserted] = m_archetypes.try_emplace(newSig, std::make_shared<Archetype>(Archetype(newSig, createSizes())));
		Archetype& oldArch = *rec.archetype;
		rec.archetype = archetypeIt->second; // Update entity record with updated archetype.

		// If the archetype was just created this means we have to update the graph's edges
		if (inserted)
		{
			/* ==== Case 1 ====
			* Previous node has edges to supersets of the new archetype
			* The new node should replace all imediate connections
			* eg: given the following graph
			* 
			* [] --------- [BC]
			*    \-------- [BE]
			* 
			* After adding B
			* 
			* [] - [B] --- [BC]
			*           \- [BE]
			* 
			* That is because both [BC] and [BE] are supersets of [B].
			*/

			/* ==== Case 2 ====
			* Previous node has no edges to supersets of the new archetype
			* The new node should be added as a new connection
			* eg: given the following graph
			* 
			* [] --- [A] -- [...] -- [AC]
			* 
			* After adding C
			*
			* [] --- [A] -- [...] -- [AC]
			*    \-- [C] ----------/
			* 
			* Problem is new node C should also point to AC which might not be an end node.
			* Sollution: keep track of all end nodes, retrieve the ones that are a superset of C
			* follow the edges that are subsets of C (cache searched nodes to avoid already searched paths)
			* NOTE: there may be more than 1 subsets that need to be linked (eg: if we also had [CD])
			* keep iterating through the graph till there are no more subset edges, these are the nodes that
			* need linking with C.
			*/
		}

		std::vector<std::tuple<ID, void*>> transferedComponents = { {GET_TYPE_ID(Component, std::decay_t<decltype(components)>), &components} ... };
		for (auto id : oldSig.getTypeIDs())
		{
			void* comp = oldArch.getComponent(rec.entityIndex, id);
			transferedComponents.push_back({ id, comp });
		}

		// Populate the component storage of the archetype with added component data.
		uint32_t index = rec.archetype->addEntity(transferedComponents);
		rec.entityIndex = index;

		// Remove the components from the old archetype (if not NULL set)
		if (oldSig.getCount() > 0)
		{
			// [A,C] + [B] -> [A,B,C]

			// TODO:
			// Remove uses swap idiom, meaning we move last element to deleted position.
			// Therefore we have to update the entity record of the swapped element to reflect
			// the updated position.

			rec.archetype->removeEntity(rec.entityIndex);
		}

	}

private:
	std::unordered_map<CompSignature, std::shared_ptr<Archetype>> m_archetypes;
	std::unordered_map<ID, EntityRecord> m_entityRecords;
	std::unordered_map<ID, size_t> m_typeIDSizes;
};

} // Engine

#endif // !WORLD_HPP