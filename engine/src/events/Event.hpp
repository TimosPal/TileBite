#ifndef EVENT_HPP
#define EVENT_HPP

#include "utilities/Identifiable.hpp"

namespace Engine {

class IEvent : public IIdentifiable {
public:
	virtual ~IEvent() = default;
};

template<typename EventType>
class Event : public IEvent, public Identifiable<Event, EventType> {
public:
	const ID getInstanceTypeID() override { return Identifiable<Event, EventType>::getInstanceTypeID(); }
	const ID getInstanceID() override { return Identifiable<Event, EventType>::getInstanceID(); }
};

} // Engine

#endif !EVENT_HPP