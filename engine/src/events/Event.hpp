#ifndef EVENT_HPP
#define EVENT_HPP

#include "utilities/Identifiable.hpp"

namespace Engine {

class IEvent : public IIdentifiable {
public:
	IEvent(bool isBlocking) : m_isBlocking(isBlocking) {}
	virtual ~IEvent() = default;

	bool getIsBlocking() const { return m_isBlocking; }
private:
	bool m_isBlocking;
};

template<typename EventType>
class Event : public IEvent, public Identifiable<Event, EventType> {
public:
	Event(bool isBlocking = true) : IEvent(isBlocking) {}
	const ID getInstanceTypeID() override { return Identifiable<Event, EventType>::getInstanceTypeID(); }
	const ID getInstanceID() override { return Identifiable<Event, EventType>::getInstanceID(); }
};

} // Engine

#endif // !EVENT_HPP