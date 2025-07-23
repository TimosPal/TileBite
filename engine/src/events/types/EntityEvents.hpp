#ifndef ENTITY_EVENTS_HPP
#define ENTITY_EVENTS_HPP

#include "events/Event.hpp"

namespace Engine {

class EntityAddComponentEvent : public Event {
	SETUP_ID(Event, EntityAddComponentEvent)
public:
	EntityAddComponentEvent(ID id) : Engine::Event(true), m_entityID(id) {}

	ID getEntityID() const { return m_entityID; }
private:
	ID m_entityID;
};

class EntityRemoveEvent : public Event {
	SETUP_ID(Event, EntityRemoveEvent)
public:
	EntityRemoveEvent(ID id) : Engine::Event(true), m_entityID(id) {}

	ID getEntityID() const { return m_entityID; }
private:
	ID m_entityID;
};

} // Engine

#endif // !ENTITY_EVENTS_HPP
