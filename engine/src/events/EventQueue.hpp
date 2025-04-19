#ifndef EVENT_QUEUE_HPP
#define EVENT_QUEUE_HPP

#include <memory>
#include <queue>

#include "events/Event.hpp"
#include "events/EventDispatcher.hpp"

namespace Engine {

class EventQueue {
public:
	template<typename EventType, typename... Args>
	requires DerivedFrom<EventType, IEvent>
	void add(Args&&... args)
	{
		m_events.push(std::make_unique<EventType>(std::forward<Args>(args)...));
	}

	void dispatchAll(EventDispatcher& dispatcher);
private:
	std::queue<std::unique_ptr<IEvent>> m_events;
};

} // Engine

#endif // !EVENT_QUEUE_HPP