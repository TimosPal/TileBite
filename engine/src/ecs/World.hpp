#ifndef WORLD_HPP
#define WORLD_HPP

#include "core/pch.hpp"
#include "core/Types.hpp"
#include "utilities/Identifiable.hpp"
#include "ecs/Archetype.hpp"
#include "ecs/Signature.hpp"
#include "ecs/QueryResponse.hpp"

namespace Engine {

class Scene; // Forward declaration for friendship.

// Can retrieve an entity row from the ComponentStorage given
// an archetype reference.
struct EntityRecord {
	uint32_t entityIndex;
	std::shared_ptr<Archetype> archetype;
};

static constexpr size_t DEFAULT_ARCHETYPES_SIZE = 128;

// The World class is responsible for managing entities and their components.
class World {
public:
	// NOTE: using friendship to avoid making a wrapper view class for a single function 
	// If more private API is needed then a wrapper class should be created.
	friend class Scene; // Allow Scene to access executeDeferredActions.

	// NOTE: createEntity and addComponents are delaying actions by adding them to a deferredActions vector.
	// This way systems can change the world state without causing inconsistencies. The world needs to run the
	// actions in the next update.

	ID createEntity();

	template <typename ComponentType>
	ComponentType* getComponent(ID entityID)
	{
		auto entityIt = m_entityRecords.find(entityID);
		ASSERT(entityIt != m_entityRecords.end(), "Entity not found");

		EntityRecord& rec = entityIt->second;
		uint32_t componentIndex = rec.archetype->getSignature().getIndex(GET_TYPE_ID(Component, std::decay_t<ComponentType>));
		void* comp = rec.archetype->getComponent(rec.entityIndex, componentIndex);
		return reinterpret_cast<ComponentType*>(comp);
	}

	// Excecutions of adds are delayed till next update to avoid incosistencies when
	// systems change world states.
	template <typename ...ComponentTypes>
	void addComponents(ID entityID, ComponentTypes&&... components) {
		// Capture components by forwarding each of them separately
		m_deferredActions.push_back([this, entityID, ...components = std::forward<ComponentTypes>(components)]() mutable {
			// Forward the components correctly to addImpl
			addComponentsImpl(entityID, std::forward<ComponentTypes>(components)...);
		});
	}

	template <typename ...ComponentTypes>
	QueryResponse<ComponentTypes...> query()
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
		std::vector<std::shared_ptr<Archetype>> queryArchetypes;
		for (ID id : intersection.getSetBits())
		{
			queryArchetypes.push_back(m_archetypesByID[id]);
		}

		return QueryResponse<ComponentTypes...>(std::move(queryArchetypes));
	}

private:
	void executeDeferredActions();

	void createEntityImpl(ID entityID);

	template <typename ...ComponentTypes>
	void addComponentsImpl(ID entityID, ComponentTypes&&... components)
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

	// Store actions to avoid incosistencies when systems change world states.
	std::vector<std::function<void()>> m_deferredActions;

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