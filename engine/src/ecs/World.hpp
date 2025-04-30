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

static constexpr size_t DEFAULT_ARCHETYPES_SIZE = 128;

class World {
public:
	ID createEntity();

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
		std::vector<ID> newTypeIDs = { GET_TYPE_ID(Component, std::decay_t<ComponentTypes>)... };
		ASSERT(
			newTypeIDs.size() == std::set<ID>(newTypeIDs.begin(), newTypeIDs.end()).size(),
			"ComponentTypes must be unique"
		);

		auto entityIt = m_entityRecords.find(entityID);
		ASSERT(entityIt != m_entityRecords.end(), "Entity not found");

		EntityRecord& rec = entityIt->second;
		Signature& oldSig = rec.archetype->getSignature();
		Signature addedSig = Signature(newTypeIDs);
		ASSERT(oldSig.commonBits(addedSig).popCount() == 0, "A component was already added");
		Signature newSig = oldSig + addedSig;

		// Add component sizes to map.
		((m_typeIDSizes[GET_TYPE_ID(Component, std::decay_t<decltype(components)>)] = sizeof(components)), ...);
		Archetype& oldArch = *rec.archetype;
		rec.archetype = getArchetype(newSig); // Update entity record with updated archetype.

		// Build new vector by mixing old and new components.
		std::vector<std::tuple<ID, void*>> transferedComponents = {
			{GET_TYPE_ID(Component, std::decay_t<decltype(components)>), &components} ... 
		};
		for (auto id : oldSig.getTypeIDs())
		{

			void* comp = oldArch.getComponent(rec.entityIndex, id);
			transferedComponents.push_back({ id, comp });
		}

		// Populate the component storage of the archetype with added component data.
		uint32_t index = rec.archetype->addEntity(transferedComponents, entityID);

		// Remove uses swap idiom, meaning we move last element to deleted position.
		// Therefore we have to update the entity record of the swapped element to reflect
		// the updated position.
		ID* swappedID = oldArch.removeEntity(rec.entityIndex);
		if (swappedID)
		{
			// If the last element was removed then there is no need
			// to update any records since no swapping happend.
			auto swappedIt = m_entityRecords.find(*swappedID);
			ASSERT(swappedIt != m_entityRecords.end(), "Entity not found");
			EntityRecord& swappedRec = swappedIt->second;
			swappedRec.entityIndex = rec.entityIndex;
		}

		rec.entityIndex = index;
	}

	template <typename ...ComponentTypes>
	Bitset query()
	{
		std::vector<ID> typeIDs = { GET_TYPE_ID(Component, std::decay_t<ComponentTypes>) ... };
		ASSERT(
			typeIDs.size() == std::set<ID>(typeIDs.begin(), typeIDs.end()).size(),
			"ComponentTypes must be unique"
		);

		Bitset intersection = m_existingArchetypes;
		for (ID id : typeIDs)
		{
			auto it = m_archetypeIndexes.find(id);
			Bitset archBitset = (it != m_archetypeIndexes.end()) ? it->second : Bitset(DEFAULT_ARCHETYPES_SIZE);
			intersection &= archBitset;
		}

		// Set bits in the intersection bitset represent
		// archetype indexes that are a superset of the query.
		for (ID id : intersection.getSetBits())
		{
			Archetype& currArch = *m_archetypesByID[id];
			currArch.getSignature().getTypeIDs();
		}

		return intersection;
	}

private:
	std::unordered_map<Signature, std::shared_ptr<Archetype>> m_archetypes;
	std::unordered_map<ID, EntityRecord> m_entityRecords;
	std::unordered_map<ID, size_t> m_typeIDSizes;

	// ==========================
	// Query helper structures.
	// ==========================
	// Each archetype has a unique ID 1,2,...,N
	// This ID can be stored in a bitset of size N.
	// If for every componentID we store the archetype ID
	// by enabling the appropriate bit in a stored bitset we can
	// easily find archetype supersets of a query by calculating the
	// intersection of all the bit fields and then iterating on said indices.

	// ComponentID -> Signature with archetype indexes.
	std::unordered_map<ID, Bitset> m_archetypeIndexes;
	// Archetype ID -> Archetype
	std::unordered_map<ID, std::shared_ptr<Archetype>> m_archetypesByID;
	Bitset m_existingArchetypes{ DEFAULT_ARCHETYPES_SIZE, false };

	std::shared_ptr<Archetype> getArchetype(Signature& sig);
};

} // Engine

#endif // !WORLD_HPP