#ifndef KEY_EVENT_HPP
#define KEY_EVENT_HPP

#include "events/Event.hpp"

namespace Engine {

class KeyEvent : public Event {
public:
	KeyEvent(int keyCode) 
		: m_keyCode(keyCode), Engine::Event(true)
	{}

	int getKeyCode() const { return m_keyCode; }
protected:
	int m_keyCode;
};

class KeyPressedEvent : public KeyEvent {
	SETUP_ID(Event, KeyPressedEvent)
public:
	KeyPressedEvent(int keyCode) 
		: KeyEvent(keyCode)
	{}
};

class KeyReleasedEvent : public KeyEvent {
	SETUP_ID(Event, KeyReleasedEvent)
public:
	KeyReleasedEvent(int keyCode)
		: KeyEvent(keyCode)
	{}
};

class KeyRepeatEvent : public KeyEvent {
	SETUP_ID(Event, KeyRepeatEvent)
public:
	KeyRepeatEvent(int keyCode)
		: KeyEvent(keyCode)
	{}
};

} // Engine

#endif // !KEY_EVENT_HPP
