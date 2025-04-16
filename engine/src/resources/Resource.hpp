#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <cstdint>
#include <algorithm>

#include "utilities/Assertions.hpp"
#include "utilities/Identifiable.hpp"
#include "utilities/Concepts.hpp"

namespace Engine {

// ResourceType must be derived from Resource for this to compile.
template<typename ResourceType>
class Resource : public Identifiable<Resource, ResourceType> {
public:
	void watch() { m_watchers++; }
	void unwatch() { if (m_watchers > 0) --m_watchers; }

	bool create()
	{
		if (m_isCreated) return true;
		m_isCreated = createImplementation();
		return m_isCreated;
	}

	// Resource is destroyed only if there are no watchers left.
	void destroy()
	{
		if (!m_isCreated || m_watchers > 0) return;
		m_isCreated = false;
		destroyImplementation();
	}
protected:
	// Should implement creation and deletion logic for derived Resource types.
	virtual bool createImplementation() { return false; };
	virtual bool destroyImplementation() { return false; };

	Resource() = default;
	~Resource()
	{ 
		ASSERT(m_watchers == 0);
		if(m_isCreated) destroyImplementation();
	}
private:
	uint32_t m_watchers = 0;
	bool m_isCreated = false;
};

} // Engine

#endif // !RESOURCE_HPP