#ifndef EVENT_CALLBACK_HPP
#define EVENT_CALLBACK_HPP

#include "events/Event.hpp"
#include "utilities/Identifiable.hpp"

namespace Engine {

template<typename EventType>
requires DerivedFrom<EventType, IEvent>
class EventCallback : public Identifiable<EventCallback, EventType> {
public:
	using Callback = std::function<void(const EventType&)>;

	EventCallback(Callback&& callback) : m_callback(std::move(callback)) {}

	const Callback& getCallback() const { return m_callback; }
private:
	Callback m_callback;
};

} // Engine

#endif // !EVENT_CALLBACK_HPP