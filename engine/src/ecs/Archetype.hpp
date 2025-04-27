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

    Archetype(CompSignature& sig, std::vector<size_t>&& componentSizes);

    uint32_t addEntity(std::vector<std::tuple<ID, void*>> components, ID entityID);
    ID removeEntity(uint32_t index);
    void* getComponent(uint32_t entityIndex, uint32_t componentIndex);

    CompSignature& getSignature() { return m_signature; }

private:
    // Contiguous blocks of memmory for each component type,
    // stored in SoA fashion.
    std::vector<ComponentStorage> m_components;
    // Edges represent connection between added and removed components
    // eg: [A] <- [A, B] -> [A, B, C] (add = C) (remove = B)  
    // They may represent more complex additions or removals hence
    // signatures are used to reflect the changed state.
    // Structure: [ComponentID, Edge]
    std::unordered_map<CompSignature, ArchetypeEdge> m_edges;
    // Entity IDs
	std::vector<ID> m_entityIDs;

    CompSignature m_signature;

    uint32_t m_entitiesCount = 0;
};

} // Engine

#endif // !ARCHETYPE_HPP