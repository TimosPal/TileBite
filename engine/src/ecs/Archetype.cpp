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
    return m_components[componentIndex].get(entityIndex);
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

ID Archetype::removeEntity(uint32_t index)
{
    bool removed = removeElement(m_entityIDs, index);
	ASSERT(removed, "Index out of bounds");
    for (auto& componentStorage : m_components)
    {
        componentStorage.remove(index);
    }

	// Return the ID of the swapped entity
	return m_entityIDs[index];
}

} // Engine