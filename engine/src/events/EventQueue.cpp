#include "events/EventQueue.hpp"

namespace Engine {

void EventQueue::addEventHandler(std::function<void(Event&)> handler)
{
	m_eventHandlers.push_back(handler);
}

void EventQueue::push(std::unique_ptr<Event> event)
{
	m_events.push(std::move(event));
}

void EventQueue::dispatchAll(LayerStack& layers)
{
    // Move to new queue so that if an event adds a new event it will be 
    // processed in the next iteration.
    std::queue<std::unique_ptr<Event>> dispatchEvents;
    dispatchEvents.swap(m_events);

	while (!dispatchEvents.empty())
	{
		std::unique_ptr<Event> event = std::move(dispatchEvents.front());
        dispatchEvents.pop();

		for(auto handler : m_eventHandlers)
		{
			if (event->isHandled()) break;
			handler(*event);
		}
	}
}

} // Engine