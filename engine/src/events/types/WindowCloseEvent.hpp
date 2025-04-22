#ifndef WINDOW_CLOSE_EVENT_HPP
#define WINDOW_CLOSE_EVENT_HPP

#include "events/Event.hpp"

namespace Engine {

class WindowCloseEvent : public Event {
	SETUP_ID(Event, WindowCloseEvent)
public:
	WindowCloseEvent() : Engine::Event(true) {}
};

} // Engine

#endif // !WINDOW_CLOSE_EVENT_HPP
