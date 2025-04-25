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

    Archetype(CompSignature& sig) 
        : m_signature(sig), m_components(sig.getCount())
    {}

    template <typename ComponentType>
    ComponentType* getComponent(uint32_t entityIndex)
    {
        uint32_t componentIndex = m_signature.getIndex(GET_TYPE_ID(Component, ComponentType));
        return m_components[componentIndex].get<ComponentType>(entityIndex);
    }

    template <typename ...ComponentTypes>
    uint32_t addEntity(ComponentTypes&&... components)
    {
        ASSERT(
            CompSignature(GET_TYPE_ID(Component, std::decay_t<decltype(components)>) ...) == m_signature,
            "Invalid components signature"
        );

        auto addOne = [&](auto&& comp)
        {
            using ComponentType = std::decay_t<decltype(comp)>;
            ID compTypeID = GET_TYPE_ID(Component, ComponentType);
            auto index = m_signature.getIndex(compTypeID);
            m_components[index].add(comp);
        };
        (addOne(std::forward<ComponentTypes>(components)), ...);

        return m_entitiesCount++;
    }

    void removeEntity(uint32_t index)
    {

    }

    const CompSignature& getSignature() const { return m_signature; }

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

    CompSignature m_signature;

    uint32_t m_entitiesCount = 0;
};

} // Engine

#endif // !ARCHETYPE_HPP