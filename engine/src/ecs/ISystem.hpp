#ifndef ISYSTEM_HPP
#define ISYSTEM_HPP

#include "core/pch.hpp"
#include "ecs/World.hpp"
#include "resources/AssetsManager.hpp"

namespace Engine {

class ISystem {
public:

    virtual ~ISystem() = default;
    virtual void update(float deltaTime) = 0;

    World* getWorld()
    {
        ASSERT(m_world != nullptr, "World not injected");
        return m_world;
    }

    AssetsManager* getAssetsManager()
    {
        ASSERT(m_world != nullptr, "Assets manager not injected");
        return m_assetsManager;
    }

    // NOTE: this dependency injection is abstracted to make client side code easier to use
    void setWorld(World* world) { m_world = world; }
    void setAssetsManager(AssetsManager* assets) { m_assetsManager = assets; }
private:
    World* m_world = nullptr;
    AssetsManager* m_assetsManager = nullptr;
};

} // Engine

#endif // !ISYSTEM_HPP