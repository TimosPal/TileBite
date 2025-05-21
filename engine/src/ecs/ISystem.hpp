#ifndef ISYSTEM_HPP
#define ISYSTEM_HPP

#include "core/pch.hpp"
#include "ecs/World.hpp"
#include "resources/AssetsManager.hpp"

namespace Engine {

class ISystem {
public:
    void setWorld(World* world) { m_world = world; }
    void setAssetsManager(AssetsManager* assets) { m_assetsManager = assets; }

    virtual ~ISystem() = default;
    virtual void update(float deltaTime) = 0;
protected:
    World* m_world = nullptr;
    AssetsManager* m_assetsManager = nullptr;
};

} // Engine

#endif // !ISYSTEM_HPP