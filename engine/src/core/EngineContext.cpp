#include "core/EngineContext.hpp"

#include "resources/AssetsManager.hpp"
#include "scenes/SceneManager.hpp"
#include "events/EventDispatcher.hpp"
#include "input/InputManager.hpp"

namespace Engine {

EventDispatcher& InjectEngineContext::getCoreEventDispatcher()
{
	ASSERT(m_engineContext != nullptr, "Engine context not injected");
	return *m_engineContext->EventDispatcherPtr;
}

SceneManager& InjectEngineContext::getSceneManager()
{
	ASSERT(m_engineContext != nullptr, "Engine context not injected");
	return *m_engineContext->SceneManagerPtr;
}

AssetsManager& InjectEngineContext::getAssetsManager()
{
	ASSERT(m_engineContext != nullptr, "Engine context not injected");
	return *m_engineContext->AssetsManagerPtr;
}

void InjectEngineContext::pushEvent(std::unique_ptr<Event> event)
{
	ASSERT(m_engineContext != nullptr, "Engine context not injected");
	m_engineContext->pushEventCallable(std::move(event));
}

InputManager& InjectEngineContext::getInputManager()
{
	ASSERT(m_engineContext != nullptr, "Engine context not injected");
	return *m_engineContext->InputManagerPtr;
}

Window& InjectEngineContext::getWindow()
{
	ASSERT(m_engineContext != nullptr, "Engine context not injected");
	return *m_engineContext->WindowPtr;
}

} // Engine