#ifndef MOUSE_EVENT_HPP
#define MOUSE_EVENT_HPP

#include "events/Event.hpp"

namespace Engine {

class MouseKeyEvent : public Event {
public:
	MouseKeyEvent(int keyCode)
		: m_keyCode(keyCode), Engine::Event(true)
	{}
protected:
	int m_keyCode;
};

class MouseKeyPressedEvent : public MouseKeyEvent {
	SETUP_ID(Event, MouseKeyPressedEvent)
public:
	MouseKeyPressedEvent(int keyCode)
		: MouseKeyEvent(keyCode)
	{}
};

class MouseKeyReleasedEvent : public MouseKeyEvent {
	SETUP_ID(Event, MouseKeyReleasedEvent)
public:
	MouseKeyReleasedEvent(int keyCode)
		: MouseKeyEvent(keyCode)
	{}
};

class MouseMovementEvent : public Event {
	SETUP_ID(Event, MouseMovementEvent)
public:
	MouseMovementEvent(float x, float y)
		: m_x(x), m_y(y), Event(true)
	{}

	float getX() const { return m_x; }
	float getY() const { return m_y; }
private:
	float m_x;
	float m_y;
};

class MouseScrollEvent : public Event {
	SETUP_ID(Event, MouseScrollEvent)
public:
	MouseScrollEvent(float xOffset, float yOffset)
		: m_xOffset(xOffset), m_yOffset(yOffset), Event(true)
	{}

	float getXOffset() const { return m_xOffset; }
	float getYOffset() const { return m_yOffset; }
private:
	float m_xOffset;
	float m_yOffset;
};

} // Engine

#endif // !MOUSE_EVENT_HPP
