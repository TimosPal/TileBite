#ifndef LAYER_HPP
#define LAYER_HPP

#include "core/pch.hpp"
#include "events/Event.hpp"
#include "events/EventDispatcher.hpp"

namespace Engine {

// Each layer holds its own event dispatcher.
// Event listeners subscribe to layers.
class Layer {
public:
	virtual void onAttach() {}
	virtual void onDetach() {}
	virtual void onUpdate() {}

	void onEvent(Event& event) 
	{
		m_eventDispatcher.dispatch(event);
	}

protected:
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
private:
	EventDispatcher m_eventDispatcher;
};

} // Engine

#endif // !LAYER_HPP
