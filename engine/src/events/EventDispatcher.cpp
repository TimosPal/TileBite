#include "events/EventDispatcher.hpp"

namespace Engine {

void EventDispatcher::dispatch(Event& event)
{
    ID eventID = event.getTypeID(); // Get the event type ID
    auto it = m_listeners.find(eventID);
    if (it == m_listeners.end()) return;

    // Dispatch the event to all registered listeners
    for (auto& [callbackID, callback] : it->second)
    {
        callback(event);
    }
}

} // Engine 