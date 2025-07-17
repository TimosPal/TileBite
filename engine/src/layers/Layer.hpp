#ifndef LAYER_HPP
#define LAYER_HPP

#include "core/pch.hpp"
#include "events/Event.hpp"
#include "events/EventDispatcher.hpp"
#include "ecs/SystemManager.hpp"
#include "resources/AssetsManager.hpp"
#include "scenes/SceneManager.hpp"
#include "core/EngineContext.hpp"

namespace Engine {

// Each layer holds its own event dispatcher.
// Event listeners subscribe to layers.
class Layer : public InjectEngineContext {
public:
	Layer(std::string name = "NoName") :
		m_eventDispatcher(),
		m_systemManager(),
		m_name(std::move(name))
	{}

	void init()
	{
		m_systemManager.setAssetsManager(&getAssetsManager());
		m_systemManager.setSceneManager(&getSceneManager());
		m_systemManager.setPushEventCallable(getPushEventCallable());
	}

	virtual void onAttach() {}
	virtual void onDetach() {}

	void enable() { m_isEnabled = true; }
	void disable() { m_isEnabled = false; }

	const std::string& getName() const { return m_name; }

	virtual void onUpdate(float deltaTime)
	{
		if(!m_isEnabled)
			return;

		// Update systems.
		m_systemManager.updateSystems(deltaTime);
	}

	virtual void onEvent(Event& event) 
	{
		if (!m_isEnabled)
			return;

		m_eventDispatcher.dispatch(event);
	}

protected:
	// TODO: make a more protective interface for client side use.
	SystemManager& getSystemManager() { return m_systemManager; }
	EventDispatcher& getEventDispatcher() { return m_eventDispatcher; }
private:
	EventDispatcher m_eventDispatcher;
	SystemManager m_systemManager;

	bool m_isEnabled = true;
	std::string m_name;
};

} // Engine

#endif // !LAYER_HPP
