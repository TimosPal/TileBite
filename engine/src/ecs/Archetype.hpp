#ifndef ARCHETYPE_HPP
#define ARCHETYPE_HPP

#include "core/pch.hpp"
#include "ecs/Signature.hpp"
#include "ecs/ComponentStorage.hpp"
#include "core/Types.hpp"

namespace Engine {

class Archetype {
public:
    struct ArchetypeEdge {
        Archetype* superset = nullptr; // Null if last
        Archetype* subset = nullptr; // Null if first
    };

    Archetype(Signature& sig, std::vector<size_t>&& componentSizes);

    uint32_t addEntity(std::vector<std::tuple<ID, void*>> components, ID entityID);
    ID* removeEntity(uint32_t index);
    void* getComponent(uint32_t entityIndex, uint32_t componentIndex);

    Signature& getSignature() { return m_signature; }

	uint32_t getEntitiesCount() const { return m_entitiesCount; }

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