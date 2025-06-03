#ifndef ARCHETYPE_HPP
#define ARCHETYPE_HPP

#include "core/pch.hpp"
#include "ecs/Signature.hpp"
#include "ecs/ComponentStorage.hpp"
#include "core/Types.hpp"
#include "utilities/Identifiable.hpp"

namespace Engine {

class Archetype : public Identifiable {
	SETUP_ID(Archetype, Archetype)
public:
    Archetype(Signature& sig, std::vector<std::tuple<ID, size_t>>&& componentSizes);

    uint32_t addEntity(std::vector<std::tuple<ID, void*>> components, ID entityID);
    ID* removeEntity(uint32_t index);
    void* getComponent(uint32_t entityIndex, uint32_t componentIndex);
	std::vector<ComponentStorage>& getComponents() { return m_components; }

    Signature& getSignature() { return m_signature; }

	uint32_t getEntitiesCount() const { return m_entitiesCount; }

	std::vector<ID>& getEntityIDs() { return m_entityIDs; }

	template <typename ...ComponentTypes>
    class Iterator;

    template<typename... T>
    auto begin()
    {
        return Iterator<T...>(this, 0);
    }

    template<typename... T>
    auto end()
    {
        return Iterator<T...>(this, getEntitiesCount());
    }

private:
    // Contiguous blocks of memmory for each component type,
    // stored in SoA fashion.
    std::vector<ComponentStorage> m_components;
    // Entity IDs
	std::vector<ID> m_entityIDs;

    Signature m_signature;

    uint32_t m_entitiesCount = 0;
};

} // Engine

#endif // !ARCHETYPE_HPP