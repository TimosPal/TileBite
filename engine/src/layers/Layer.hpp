#ifndef LAYER_HPP
#define LAYER_HPP

#include "core/pch.hpp"
#include "events/Event.hpp"
#include "events/EventDispatcher.hpp"
#include "ecs/SystemManager.hpp"
#include "resources/AssetsManager.hpp"
#include "scenes/SceneManager.hpp"

namespace Engine {

// Each layer holds its own event dispatcher.
// Event listeners subscribe to layers.
class Layer {
public:
	Layer() :
		m_eventDispatcher(),
		m_systemManager()
	{}

	virtual void onAttach() {}
	virtual void onDetach() {}

	virtual void onUpdate(float deltaTime)
	{
		// Update systems.
		m_systemManager.updateSystems(getSceneManager(), deltaTime);
	}

	virtual void onEvent(Event& event) 
	{
		m_eventDispatcher.dispatch(event);
	}

	// NOTE: this dependency injection is abstracted to make client side code easier to use
	void setSceneManager(SceneManager* sceneManager) { m_sceneManager = sceneManager; }
	void setAssetsManager(AssetsManager* assets) { m_assetsManager = assets; }

protected:
	SceneManager& getSceneManager()
	{ 
		ASSERT(m_sceneManager != nullptr, "Scene manager not injected");
		return *m_sceneManager;
	}

	AssetsManager& getAssetsManager()
	{
		ASSERT(m_assetsManager != nullptr, "Assets manager not injected");
		return *m_assetsManager;
	}

	template<typename EventType>
	requires DerivedFrom<EventType, Event>
	void subscribe(EventCallback<EventType>& eventCallback)
	{
		m_eventDispatcher.subscribe<EventType>(eventCallback);
	}

	template<typename EventType>
	requires DerivedFrom<EventType, Event>
	void unsubscribe(EventCallback<EventType>& eventCallback)
	{
		m_eventDispatcher.unsubscribe<EventType>(eventCallback);
	}

	void addSystem(std::unique_ptr<ISystem> system)
	{
		// Inject world and asset manager for hidden client side use
		// (Removes the need for client side construction to include injections)
		system->setAssetsManager(&getAssetsManager());
		system->setSceneManager(&getSceneManager());

		m_systemManager.addSystem(std::move(system));
	}

private:
	EventDispatcher m_eventDispatcher;
	SystemManager m_systemManager;
	
	SceneManager* m_sceneManager;
	AssetsManager* m_assetsManager;
};

} // Engine

#endif // !LAYER_HPP
