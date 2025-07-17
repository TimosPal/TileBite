#include "core/EngineContext.hpp"
#include "resources/AssetsManager.hpp"
#include "scenes/SceneManager.hpp"

namespace Engine {

void InjectEngineContext::setSceneManager(SceneManager* sceneManager) {
	m_sceneManager = sceneManager;
}

void InjectEngineContext::setAssetsManager(AssetsManager* assets) {
	m_assetsManager = assets;
}

void InjectEngineContext::setPushEventCallable(std::function<void(std::unique_ptr<Event>)> pushEventCallable) {
	m_pushEventCallable = std::move(pushEventCallable);
}

SceneManager& InjectEngineContext::getSceneManager() {
	ASSERT(m_sceneManager != nullptr, "Scene manager not injected");
	return *m_sceneManager;
}

AssetsManager& InjectEngineContext::getAssetsManager() {
	ASSERT(m_assetsManager != nullptr, "Assets manager not injected");
	return *m_assetsManager;
}

void InjectEngineContext::pushEvent(std::unique_ptr<Event> event) {
	m_pushEventCallable(std::move(event));
}

std::function<void(std::unique_ptr<Event>)> InjectEngineContext::getPushEventCallable() {
	return m_pushEventCallable;
}

} // Engine