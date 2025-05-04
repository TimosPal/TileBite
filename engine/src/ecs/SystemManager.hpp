#ifndef SYSTEM_MANAGER_HPP
#define SYSTEM_MANAGER_HPP

#include "core/pch.hpp"
#include "ecs/ISystem.hpp"
#include "ecs/World.hpp"

namespace Engine {

class SystemManager {
public:
    SystemManager(World& world) : m_world(world) {}

    void addSystem(std::unique_ptr<ISystem> system)
    {
        m_systems.push_back(std::move(system));
    }

    void updateSystems(float deltaTime)
    {
        for (auto& system : m_systems)
        {
            system->update(m_world, deltaTime);
        }

        m_world.executeDeferredActions();
    }

private:
    std::vector<std::unique_ptr<ISystem>> m_systems;
    World& m_world;
};

} // Engine

#endif // !SYSTEM_MANAGER_HPP