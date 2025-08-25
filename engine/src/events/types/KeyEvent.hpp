#ifndef KEY_EVENT_HPP
#define KEY_EVENT_HPP

#include "events/Event.hpp"

namespace TileBite {

class KeyEvent : public Event {
public:
	KeyEvent(int keyCode) 
		: m_keyCode(keyCode), TileBite::Event(false)
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

} // TileBite

#endif // !KEY_EVENT_HPP
