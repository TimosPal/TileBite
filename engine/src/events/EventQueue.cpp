#include "events/EventQueue.hpp"

namespace Engine {

void EventQueue::dispatchAll(EventDispatcher& dispatcher)
{
    // Move to new queue so that if an event adds a new event it will be 
    // processed in the next iteration.
    std::queue<std::unique_ptr<IEvent>> dispatchEvents;
    dispatchEvents.swap(m_events);

	while (!dispatchEvents.empty())
	{
		std::unique_ptr<IEvent> event = std::move(dispatchEvents.front());
        dispatchEvents.pop();

        dispatcher.dispatch(*event);
	}
}

} // Engine