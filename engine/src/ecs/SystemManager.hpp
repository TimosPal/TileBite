#ifndef SYSTEM_MANAGER_HPP
#define SYSTEM_MANAGER_HPP

#include "core/pch.hpp"
#include "ecs/ISystem.hpp"
#include "physics/PhysicsEngine.hpp"

namespace TileBite {

class SystemManager {
public:
    SystemManager() = default;

    void addSystem(std::unique_ptr<ISystem> system)
    {
		system->onAttach();
        
        m_systems.push_back(std::move(system));
    }

    void updateSystems(float deltaTime)
    {
        for (auto& system : m_systems)
        {
            system->update(deltaTime);
        }
    }

private:
    std::vector<std::unique_ptr<ISystem>> m_systems;
};

} // TileBite

#endif // !SYSTEM_MANAGER_HPP