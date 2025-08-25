#include "core/EngineContext.hpp"

#include "resources/AssetsManager.hpp"
#include "scenes/SceneManager.hpp"
#include "events/EventDispatcher.hpp"
#include "input/InputManager.hpp"
#include "window/Window.hpp"

namespace TileBite {

EventDispatcher& InjectEngineContext::getCoreEventDispatcher()
{
	ASSERT(m_engineContext != nullptr, "Engine context not injected");
	ASSERT(m_engineContext->EventDispatcherPtr != nullptr, "Event dispatcher not initialized");
	return *m_engineContext->EventDispatcherPtr;
}

SceneManager& InjectEngineContext::getSceneManager()
{
	ASSERT(m_engineContext != nullptr, "Engine context not injected");
	ASSERT(m_engineContext->SceneManagerPtr != nullptr, "Scene manager not initialized");
	return *m_engineContext->SceneManagerPtr;
}

AssetsManager& InjectEngineContext::getAssetsManager()
{
	ASSERT(m_engineContext != nullptr, "Engine context not injected");
	ASSERT(m_engineContext->AssetsManagerPtr != nullptr, "Assets manager not initialized");
	return *m_engineContext->AssetsManagerPtr;
}

void InjectEngineContext::pushEvent(std::unique_ptr<Event> event)
{
	ASSERT(m_engineContext != nullptr, "Engine context not injected");
	ASSERT(m_engineContext->pushEventCallable != nullptr, "Push event callable not set");
	m_engineContext->pushEventCallable(std::move(event));
}

InputManager& InjectEngineContext::getInputManager()
{
	ASSERT(m_engineContext != nullptr, "Engine context not injected");
	ASSERT(m_engineContext->InputManagerPtr != nullptr, "Input manager not initialized");
	return *m_engineContext->InputManagerPtr;
}

Window& InjectEngineContext::getWindow()
{
	ASSERT(m_engineContext != nullptr, "Engine context not injected");
	ASSERT(m_engineContext->WindowPtr != nullptr, "Window not initialized");
	return *m_engineContext->WindowPtr;
}

Renderer2D& InjectEngineContext::getRenderer()
{
	ASSERT(m_engineContext != nullptr, "Engine context not injected");
	ASSERT(m_engineContext->RendererPtr != nullptr, "Renderer not initialized");
	return *m_engineContext->RendererPtr;
}

} // TileBite