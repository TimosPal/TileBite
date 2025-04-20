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
class Resource : public Identifiable {
public:
	void watch() { m_watchers++; }
	void unwatch() { if (m_watchers > 0) --m_watchers; }

	bool create(bool& errorOccured)
	{
		errorOccured = false;

		if (m_isCreated) return false;
		m_isCreated = createImplementation();
		errorOccured = !m_isCreated;
		return m_isCreated;
	}

	// Resource is destroyed only if there are no watchers left.
	bool destroy(bool& errorOccured)
	{
		errorOccured = false;

		if (!m_isCreated || m_watchers > 0) return false;
		m_isCreated = destroyImplementation();
		errorOccured = !m_isCreated;
		return m_isCreated;
	}

	std::string& getName() const { return m_name; }
protected:
	// Should implement creation and deletion logic for derived Resource types.
	virtual bool createImplementation() { return false; };
	virtual bool destroyImplementation() { return false; };

	Resource(std::string name) : m_name(name) {}
	Resource() : Resource("") {}
	~Resource()
	{ 
		ASSERT(m_watchers == 0);
		if(m_isCreated) destroyImplementation();
	}
private:
	std::string m_name;

	uint32_t m_watchers = 0;
	bool m_isCreated = false;
};

} // Engine

#endif // !RESOURCE_HPP