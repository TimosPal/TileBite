#ifndef WINDOW_RESIZE_EVENT_HPP
#define WINDOW_RESIZE_EVENT_HPP

#include "events/Event.hpp"

namespace Engine {

class WindowResizeEvent : public Event {
	SETUP_ID(Event, WindowResizeEvent)
public:
	WindowResizeEvent(int width, int height) : Engine::Event(true), m_width(width), m_height(height) {}

	unsigned int getWidth() const { return m_width; }
	unsigned int getHeight() const { return m_height; }
private:
	unsigned int m_width, m_height;
};

} // Engine

#endif // !WINDOW_RESIZE_EVENT_HPP
