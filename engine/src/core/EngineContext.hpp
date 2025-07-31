#ifndef ENGINE_CONTEXT_HPP
#define ENGINE_CONTEXT_HPP

#include "core/pch.hpp"
#include "events/Event.hpp"

namespace Engine {

// Forward declarations
class SceneManager;
class AssetsManager;
class EventDispatcher;
class InputManager;

struct EngineContext {
	SceneManager* SceneManagerPtr;
	AssetsManager* AssetsManagerPtr;
	EventDispatcher* EventDispatcherPtr;
	InputManager* InputManagerPtr;
	std::function<void(std::unique_ptr<Event>)> pushEventCallable;
};

class InjectEngineContext {
public:
	void setEngineContext(EngineContext* engineContext)
	{
		m_engineContext = engineContext;
	}

	EngineContext* getEngineContext() const
	{
		return m_engineContext;
	}

protected:
	// TODO: make other interface for client side use.

	SceneManager& getSceneManager();
	AssetsManager& getAssetsManager();
	EventDispatcher& getCoreEventDispatcher();
	InputManager& getInputManager();
	void pushEvent(std::unique_ptr<Event> event);
private:
	EngineContext* m_engineContext = nullptr;
};

} // namespace Engine

#endif // ENGINE_CONTEXT_HPP
