#ifndef ISYSTEM_HPP
#define ISYSTEM_HPP

#include "core/pch.hpp"

namespace Engine {

// Forward delceration to break circular dependency from ISystem to SceneManager
class SceneManager;
class AssetsManager;

class ISystem {
public:
    virtual ~ISystem() = default;
    virtual void update(float deltaTime) = 0;

    SceneManager* getSceneManager() {
        ASSERT(m_sceneManager != nullptr, "Scene manager not injected");
        return m_sceneManager;
    }

    AssetsManager* getAssetsManager() {
        ASSERT(m_assetsManager != nullptr, "Assets manager not injected");
        return m_assetsManager;
    }

    void setSceneManager(SceneManager* sceneManager) { m_sceneManager = sceneManager; }
    void setAssetsManager(AssetsManager* assets) { m_assetsManager = assets; }

private:
    SceneManager* m_sceneManager = nullptr;
    AssetsManager* m_assetsManager = nullptr;
};

} // Engine

#endif // !ISYSTEM_HPP
