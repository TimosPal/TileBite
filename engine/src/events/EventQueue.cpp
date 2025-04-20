#include "events/EventQueue.hpp"

namespace Engine {

void EventQueue::push(std::unique_ptr<Event> event)
{
	m_events.push(std::move(event));
}

void EventQueue::dispatchAll(EventDispatcher& dispatcher)
{
    // Move to new queue so that if an event adds a new event it will be 
    // processed in the next iteration.
    std::queue<std::unique_ptr<Event>> dispatchEvents;
    dispatchEvents.swap(m_events);

	while (!dispatchEvents.empty())
	{
		std::unique_ptr<Event> event = std::move(dispatchEvents.front());
        dispatchEvents.pop();

        dispatcher.dispatch(*event);
	}
}

} // Engine