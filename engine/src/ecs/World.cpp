#include "ecs/World.hpp"

namespace Engine {

ID World::createEntity()
{
	ID entityID = GET_INSTANCE_ID(World, void); // consider global ids under World.
	Signature sig;
	auto [it, inserted] = m_archetypes.try_emplace(sig, std::make_shared<Archetype>(Archetype(sig, {})));
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
	auto [archetypeIt, inserted] = m_archetypes.try_emplace(
		sig,
		std::make_shared<Archetype>(Archetype(sig, createSizes()))
	);

	return archetypeIt->second;
}

} // Engine