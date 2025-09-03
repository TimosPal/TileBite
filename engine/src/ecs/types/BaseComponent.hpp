#ifndef BASE_COMPONENT_HPP
#define BASE_COMPONENT_HPP

#include <glm/glm.hpp>

#include "utilities/IDGenerator.hpp"

namespace TileBite {

class BaseComponent
{
public:
	BaseComponent() = default;

	bool isDirty() const { return m_isDirty; }
	void resetDirty() { m_isDirty = false; }
protected:
	void setDirty(bool dirty = true) { m_isDirty = dirty; }
private:
	bool m_isDirty = true; // NOTE: init to true for first time setup
};

} // TileBite

#endif // !BASE_COMPONENT_HPP