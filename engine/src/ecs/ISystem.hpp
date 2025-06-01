#ifndef ISYSTEM_HPP
#define ISYSTEM_HPP

#include "core/pch.hpp"
#include "resources/AssetsManager.hpp"
#include "scenes/SceneManager.hpp"

namespace Engine {

class ISystem {
public:

    virtual ~ISystem() = default;
    virtual void update(float deltaTime) = 0;

    SceneManager* getSceneManager()
    {
        ASSERT(m_sceneManager != nullptr, "Scene manager not injected");
        return m_sceneManager;
    }

    AssetsManager* getAssetsManager()
    {
        ASSERT(m_assetsManager != nullptr, "Assets manager not injected");
        return m_assetsManager;
    }

    // NOTE: this dependency injection is abstracted to make client side code easier to use
    void setSceneManager(SceneManager* sceneManager) { m_sceneManager = sceneManager; }
    void setAssetsManager(AssetsManager* assets) { m_assetsManager = assets; }
private:
    SceneManager* m_sceneManager = nullptr;
    AssetsManager* m_assetsManager = nullptr;
};

} // Engine

#endif // !ISYSTEM_HPP