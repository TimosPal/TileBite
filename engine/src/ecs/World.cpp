#include "ecs/World.hpp"

#include "ecs/SystemManager.hpp"

namespace TileBite {

void World::executeDeferredActions()
{
	for (auto& action : m_deferredActions.actions)
	{
		action();
	}
	m_deferredActions.actions.clear();
	m_deferredActions.removedEntities.clear();
	m_deferredActions.enhancedEntities.clear();
}

ID World::createEntity()
{
	// Delays the creation of the entity to avoid incosistencies when systems add 
	// entities, but does return the appropriate ID for the rest of the system to use.
	ID entityID = GET_INSTANCE_ID(World, void); // consider global ids under World.
	
	m_deferredActions.actions.push_back([this, entityID]() mutable {
		createEntityImpl(entityID);
	});

	return entityID;
}

bool World::entityExists(ID entity) const
{
	auto entityIt = m_entityRecords.find(entity);
	if (entityIt == m_entityRecords.end())
	{
		return false;
	}
	return true;
}

void World::removeEntity(ID id)
{
	if (m_deferredActions.removedEntities.find(id) != m_deferredActions.removedEntities.end())
	{
		// Avoid double removals
		return;
	}

	// Delays the deletion of the entity to avoid incosistencies for the same reason as createe
	m_deferredActions.actions.push_back([this, id]() mutable {
		removeEntityImpl(id);
	});
	m_deferredActions.removedEntities.insert(id);
}

void World::removeEntityImpl(ID id)
{
	auto entityIt = m_entityRecords.find(id);
	ASSERT(entityIt != m_entityRecords.end(), "Entity not found during removal");
	EntityRecord& rec = entityIt->second;
	std::shared_ptr<Archetype> arch = rec.archetype;
	removeEntityFromArchHelper(rec.entityIndex, *arch);
	m_entityRecords.erase(entityIt);

	if(m_removeEntityCallback) m_removeEntityCallback(id);
}

void World::removeEntityFromArchHelper(uint32_t entityIndex, Archetype& arch)
{
	// Remove uses swap idiom, meaning we move last element to deleted position.
	// Therefore we have to update the entity record of the swapped element to reflect
	// the updated position.
	ID* swappedID = arch.removeEntity(entityIndex);
	if (swappedID)
	{
		// If the last element was removed then there is no need
		// to update any records since no swapping happend.
		auto swappedIt = m_entityRecords.find(*swappedID);
		ASSERT(swappedIt != m_entityRecords.end(), "Entity not found during swapping");
		EntityRecord& swappedRec = swappedIt->second;
		swappedRec.entityIndex = entityIndex;
	}
}

void World::createEntityImpl(ID entityID)
{
	Signature sig;

	auto it = m_archetypes.find(sig);
	if (it == m_archetypes.end())
	{
		it = m_archetypes.emplace(sig, std::make_shared<Archetype>(Archetype(sig, {}))).first;
	}

	m_entityRecords.emplace(entityID, EntityRecord{ 0, it->second });
	it->second->addEntity({}, entityID); // Add empty entity to archetype.
}

std::shared_ptr<Archetype> World::getArchetype(Signature& sig)
{
	// Create sizes array for new archetype
	auto createSizes = [&]() {
		std::vector<std::tuple<ID, size_t>> result;
		result.reserve(sig.getTypeIDs().size());
		for (const auto& id : sig.getTypeIDs())
		{
			auto it = m_typeIDSizes.find(id);
			ASSERT(it != m_typeIDSizes.end(), "Size not found");
			result.push_back({sig.getIndex(id) ,it->second});
		}

		return result;
	};

	// When an entity gets a new components that means its archetype changes so we have to
	// either make a new one if no entities with such type existed, or just retrieve an already existing one
	// and append to its storage the appropriate components.
	auto archetypeIt = m_archetypes.find(sig);
	bool inserted = false;
	if (archetypeIt == m_archetypes.end())
	{
		inserted = true;
		archetypeIt = m_archetypes.emplace(sig, std::make_shared<Archetype>(Archetype(sig, createSizes()))).first;
	}

	// If the archetype was just created we update the indexes map
	// and the id based archetype map.
	if (inserted)
	{
		ID archID = archetypeIt->second->getInstanceID();
		m_archetypesByID[archID] = archetypeIt->second;

		for (ID id : sig.getTypeIDs())
		{
			auto [bitset, _] = m_archetypeIndexes.try_emplace(id, Bitset(DEFAULT_ARCHETYPES_SIZE));
			bitset->second.set(archID);
			m_existingArchetypes.set(archID);
		}
	}

	return archetypeIt->second;
}

} // TileBite