#ifndef WINDOW_CLOSE_EVENT_HPP
#define WINDOW_CLOSE_EVENT_HPP

#include "events/Event.hpp"

namespace TileBite {

class WindowCloseEvent : public Event {
	SETUP_ID(Event, WindowCloseEvent)
public:
	WindowCloseEvent() : TileBite::Event(true) {}
};

} // TileBite

#endif // !WINDOW_CLOSE_EVENT_HPP
