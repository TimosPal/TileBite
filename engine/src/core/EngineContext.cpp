#include "core/EngineContext.hpp"

#include "resources/AssetsManager.hpp"
#include "scenes/SceneManager.hpp"
#include "events/EventDispatcher.hpp"

namespace Engine {

void InjectEngineContext::setCoreEventDispatcher(EventDispatcher* coreEventDispatcher) {
	m_coreEventDispatcher = coreEventDispatcher;
}

void InjectEngineContext::setSceneManager(SceneManager* sceneManager) {
	m_sceneManager = sceneManager;
}

void InjectEngineContext::setAssetsManager(AssetsManager* assets) {
	m_assetsManager = assets;
}

void InjectEngineContext::setPushEventCallable(std::function<void(std::unique_ptr<Event>)> pushEventCallable) {
	m_pushEventCallable = std::move(pushEventCallable);
}

EventDispatcher& InjectEngineContext::getCoreEventDispatcher()
{
	ASSERT(m_coreEventDispatcher != nullptr, "Core event dispatcher not injected");
	return *m_coreEventDispatcher;
}

SceneManager& InjectEngineContext::getSceneManager()
{
	ASSERT(m_sceneManager != nullptr, "Scene manager not injected");
	return *m_sceneManager;
}

AssetsManager& InjectEngineContext::getAssetsManager()
{
	ASSERT(m_assetsManager != nullptr, "Assets manager not injected");
	return *m_assetsManager;
}

void InjectEngineContext::pushEvent(std::unique_ptr<Event> event)
{
	m_pushEventCallable(std::move(event));
}

std::function<void(std::unique_ptr<Event>)> InjectEngineContext::getPushEventCallable()
{
	ASSERT(m_pushEventCallable != nullptr, "Push event callable not initialized");
	return m_pushEventCallable;
}

} // Engine