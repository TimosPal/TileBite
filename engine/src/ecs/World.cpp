#include "ecs/World.hpp"

namespace Engine {

ID World::createEntity()
{
	ID entityID = GET_INSTANCE_ID(World, void); // consider global ids under World.
	Signature sig;

	auto it = m_archetypes.find(sig);
	if (it == m_archetypes.end())
	{
		it = m_archetypes.emplace(sig, std::make_shared<Archetype>(Archetype(sig, {}))).first;
	}

	m_entityRecords.emplace(entityID, EntityRecord{ 0, it->second });
	it->second->addEntity({}, entityID); // Add empty entity to archetype.
	return entityID;
}

std::shared_ptr<Archetype> World::getArchetype(Signature& sig)
{
	// Create sizes array for new archetype
	auto createSizes = [&]() {
		std::vector<size_t> result;
		result.reserve(sig.getTypeIDs().size());
		for (const auto& id : sig.getTypeIDs())
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

		LOG_INFO("ArchID {} -- Signature {}", archID, archetypeIt->second->getSignature().toString());

		for (ID id : sig.getTypeIDs())
		{
			auto [bitset, _] = m_archetypeIndexes.try_emplace(id, Bitset(DEFAULT_ARCHETYPES_SIZE));
			bitset->second.set(archID);
			m_existingArchetypes.set(archID);
		}
	}

	return archetypeIt->second;
}

} // Engine