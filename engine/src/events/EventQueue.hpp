#ifndef EVENT_QUEUE_HPP
#define EVENT_QUEUE_HPP

#include <memory>
#include <queue>

#include "events/Event.hpp"
#include "events/EventDispatcher.hpp"

namespace Engine {

class EventQueue {
public:
	void push(std::unique_ptr<Event> event);
	void dispatchAll(EventDispatcher& dispatcher);
private:
	std::queue<std::unique_ptr<Event>> m_events;
};

} // Engine

#endif // !EVENT_QUEUE_HPP