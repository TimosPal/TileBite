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
		auto [it, inserted] = m_archetypes.try_emplace(sig, std::make_shared<Archetype>(Archetype(sig)));
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
		return rec.archetype->getComponent<ComponentType>(rec.entityIndex);
	}

	template <typename ...ComponentTypes>
	void addComponents(ID entityID, ComponentTypes&&... components)
	{
		auto entityIt = m_entityRecords.find(entityID);
		ASSERT(entityIt != m_entityRecords.end(), "Entity not found");

		EntityRecord& rec = entityIt->second;
		const CompSignature& oldSig = rec.archetype->getSignature();
		CompSignature addedSig = CompSignature((GET_TYPE_ID(Component, std::decay_t<decltype(components)>), ...));
		ASSERT(oldSig.commonBits(addedSig) == 0, "A component was already added");
		CompSignature newSig = addedSig + oldSig;

		// When an entity gets a new components that means its archetype changes so we have to
		// either make a new one if no entities with such type existed, or just retrieve an already existing one
		// and append to its storage the appropriate components.
		auto [archetypeIt, inserted] = m_archetypes.try_emplace(newSig, std::make_shared<Archetype>(Archetype(newSig)));
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

			for (auto& [sig, arch] : m_archetypes)
			{

			}
		}

		// Populate the component storage of the archetype with added component data.
		uint32_t index = rec.archetype->addEntity(components...);
		rec.entityIndex = index;

		// Remove the components from the old archetype (if not NULL set)
		if (oldSig.getCount() > 0)
		{
			rec.archetype->removeEntity(rec.entityIndex);
		}

	}

private:
	std::unordered_map<CompSignature, std::shared_ptr<Archetype>> m_archetypes;
	std::unordered_map<ID, EntityRecord> m_entityRecords;
};

} // Engine

#endif // !WORLD_HPP