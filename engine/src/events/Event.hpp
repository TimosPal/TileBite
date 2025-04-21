#ifndef EVENT_HPP
#define EVENT_HPP

#include "utilities/Identifiable.hpp"

namespace Engine {

class Event : public Identifiable {
public:
	Event(bool isBlocking = true) : m_isBlocking(isBlocking) {}

	bool getIsBlocking() const { return m_isBlocking; }
	bool isHandled() const { return m_isHandled; }
	void consume() { m_isHandled = true; }
private:
	bool m_isBlocking;
	bool m_isHandled = false;
};

} // Engine

#endif // !EVENT_HPP