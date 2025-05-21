#ifndef LAYER_HPP
#define LAYER_HPP

#include "core/pch.hpp"
#include "events/Event.hpp"
#include "events/EventDispatcher.hpp"
#include "ecs/SystemManager.hpp"
#include "resources/AssetsManager.hpp"

namespace Engine {

// Each layer holds its own event dispatcher.
// Event listeners subscribe to layers.
class Layer {
public:
	Layer(World& world, AssetsManager& assetsManager) :
		m_eventDispatcher(),
		m_systemManager(),
		m_world(world),
		m_assetsManager(assetsManager)
	{}

	virtual void onAttach() {}
	virtual void onDetach() {}

	virtual void onUpdate(float deltaTime)
	{
		// Update systems.
		m_systemManager.updateSystems(m_world, m_assetsManager, deltaTime);
	}

	virtual void onEvent(Event& event) 
	{
		m_eventDispatcher.dispatch(event);
	}

protected:
	World& getWorld() { return m_world; }
	AssetsManager& getAssetsManager() { return m_assetsManager; }

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
		m_systemManager.addSystem(std::move(system));
	}
private:
	EventDispatcher m_eventDispatcher;
	SystemManager m_systemManager;
	World& m_world;
	AssetsManager& m_assetsManager;
};

} // Engine

#endif // !LAYER_HPP
