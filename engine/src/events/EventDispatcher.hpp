#ifndef EVENT_DISPATCHER_HPP
#define EVENT_DISPATCHER_HPP

#include "core/pch.hpp"
#include "events/Event.hpp"
#include "events/EventCallback.hpp"

namespace Engine {

class EventDispatcher {
public:
	template<typename EventType>
	requires DerivedFrom<EventType, Event>
	void subscribe(EventCallback<EventType>& eventCallback)
	{
		// Create a wrapper lambda expecting a generic IEvent arg
		// converts it to the appropriate EventType. This way the called
		// only needs to pass a function with the correct type.
		auto wrapper = [eventCallback](Event& e) {
			eventCallback.getCallback()(static_cast<EventType&>(e));
		};

		// Register callback function to active listeners.
		ID eventID = GET_TYPE_ID(Event, EventType);
		ID callbackID = eventCallback.getInstanceID();

		ASSERT(
			m_listeners[eventID].find(callbackID) == m_listeners[eventID].end(),
			"Event callback already registered"
		);
		m_listeners[eventID][callbackID] = wrapper;
	}

	template<typename EventType>
	requires DerivedFrom<EventType, Event>
	void unsubscribe(EventCallback<EventType>& eventCallback)
	{
		ID eventID = GET_TYPE_ID(Event, EventType);
		ID callbackID = eventCallback.getInstanceID();

		ASSERT(
			m_listeners[eventID].find(callbackID) != m_listeners[eventID].end(),
			"Event callback not found"
		);
		m_listeners[eventID].erase(callbackID);
	}

	void dispatch(Event& event);

private:
	// NOTE: map of maps is usefull for O(1) unsubscriptions but may lead to memory overhead.
	// Structure: [Event ID -> Callback ID -> Callback]
	using registeredCallbacks = std::unordered_map<ID, EventCallback<Event>::Callback>;
	std::unordered_map<ID, registeredCallbacks> m_listeners;
};

} // Engine

#endif !EVENT_DISPATCHER_HPP