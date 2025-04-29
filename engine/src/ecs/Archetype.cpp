#include "ecs/Archetype.hpp"

#include "utilities/misc.hpp"

namespace Engine {

Archetype::Archetype(CompSignature& sig, std::vector<size_t>&& componentSizes)
    : m_signature(sig)
{
    for (auto& componentSize : componentSizes)
    {
        m_components.push_back(ComponentStorage(componentSize));
    }
}

void* Archetype::getComponent(uint32_t entityIndex, uint32_t componentIndex)
{
	uint32_t storageComponentIndex = m_signature.getIndex(componentIndex);

	ASSERT(entityIndex < m_entitiesCount, "Entity index out of bounds");
	ASSERT(storageComponentIndex < m_signature.getCount(), "Component index out of bounds");
    return m_components[storageComponentIndex].get(entityIndex);
}

uint32_t Archetype::addEntity(std::vector<std::tuple<ID, void*>> components, ID entityID)
{
    // Make componnts signature and check if it matches the archetype signature
    auto typeIDs = [&] { std::vector<ID> ids; for (auto& [id, _] : components) ids.push_back(id); return ids; }();
    ASSERT(CompSignature(typeIDs) == m_signature, "Invalid components signature");

    for (auto& [id, component] : components)
    {
        auto index = m_signature.getIndex(id);
        m_components[index].add(component);
    }

	m_entityIDs.push_back(entityID);
    return m_entitiesCount++;
}

ID* Archetype::removeEntity(uint32_t index)
{
    bool removed = removeElement(m_entityIDs, index);
	ASSERT(removed, "Index out of bounds");
    for (auto& componentStorage : m_components)
    {
        componentStorage.remove(index);
    }
    m_entitiesCount--;

	// Return the ID of the swapped entity, or null if no swap happend.
	return (index < m_entitiesCount) ? &m_entityIDs[index] : nullptr;
}

} // Engine