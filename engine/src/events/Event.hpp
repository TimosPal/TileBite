#ifndef EVENT_HPP
#define EVENT_HPP

#include "utilities/Identifiable.hpp"

namespace Engine {

class Event : public Identifiable {
public:
	Event(bool isBlocking = true) : m_isBlocking(isBlocking) {}

	bool getIsBlocking() const { return m_isBlocking; }
private:
	bool m_isBlocking;
};

} // Engine

#endif // !EVENT_HPP