#include "ecs/World.hpp"

namespace Engine {

ID World::createEntity()
{
	ID entityID = GET_INSTANCE_ID(World, void); // consider global ids under World.
	CompSignature sig;
	auto [it, inserted] = m_archetypes.try_emplace(sig, std::make_shared<Archetype>(Archetype(sig, {})));
	m_entityRecords.emplace(entityID, EntityRecord{ 0, it->second });
	it->second->addEntity({}, entityID); // Add empty entity to archetype.

	return entityID;
}

std::shared_ptr<Archetype> World::getArchetype(CompSignature& sig)
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

	// If the archetype was just created this means we have to update the graph's edges
	if (inserted) updateArchetypeGraph();

	return archetypeIt->second;
}

void World::updateArchetypeGraph()
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

} // Engine