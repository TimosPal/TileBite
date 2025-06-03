#include "ecs/Archetype.hpp"

#include "utilities/misc.hpp"

namespace Engine {

Archetype::Archetype(Signature& sig, std::vector<std::tuple<ID, size_t>>&& componentSizes)
    : m_signature(sig)
{
    // Sort the vector based on ID to ensure proper archetype initilization.
    // NOTE: Forced to do this since ComponentStorage is not default constructible
    std::sort(componentSizes.begin(), componentSizes.end(), [](const std::tuple<ID, size_t>& a, const std::tuple<ID, size_t>& b) {
        return std::get<0>(a) < std::get<0>(b);  // Compare IDs
    });

    for (auto& [id, size] : componentSizes)
    {
        m_components.push_back(ComponentStorage(size));  // Use the correct size for initialization
    }
}

void* Archetype::getComponent(uint32_t entityIndex, uint32_t componentIndex)
{
	ASSERT(entityIndex < m_entitiesCount, "Entity index out of bounds");
	ASSERT(componentIndex < m_signature.getCount(), "Component index out of bounds");
    return m_components[componentIndex].get(entityIndex);
}

uint32_t Archetype::addEntity(std::vector<std::tuple<ID, void*>> components, ID entityID)
{
    // Make componnts signature and check if it matches the archetype signature
    auto typeIDs = [&] { std::vector<ID> ids; for (auto& [id, _] : components) ids.push_back(id); return ids; }();
    ASSERT(Signature(typeIDs) == m_signature, "Invalid components signature");

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