#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include "core/pch.hpp"
#include "utilities/assertions.hpp"
#include "utilities/Identifiable.hpp"
#include "utilities/misc.hpp"

namespace Engine {

// ResourceType must be derived from Resource for this to compile.
template<typename ResourceType>
class Resource : public Identifiable {
public:
    void watch() { m_watchers++; }
	void unwatch() { if (m_watchers > 0) --m_watchers; }

    bool create(bool& errorOccurred)
    {
        errorOccurred = false;
        if (m_isCreated) return false;

        bool success = createImplementation();
        if (!success)
        {
            errorOccurred = true;
            return false;
        }
        m_isCreated = true;
        return true;
    }

    bool destroy(bool& errorOccurred)
    {
        errorOccurred = false;
        if (!m_isCreated || m_watchers > 0) return false;

        bool success = destroyImplementation();
        if (!success)
        {
            errorOccurred = true;
            return false;
        }
        m_isCreated = false;
        return true;
    }

	std::string& getName() { return m_name; }
	bool isCreated() const { return m_isCreated; }
    virtual bool isValid() { return true; }

protected:
	// Should implement creation and deletion logic for derived Resource types.
	virtual bool createImplementation() { return false; };
	virtual bool destroyImplementation() { return false; };

	Resource(std::string name) : m_name(name) {}
	Resource() : Resource("emptyResource") {}
	~Resource()
	{ 
		ASSERT(m_watchers == 0, "Resource destroyed with active watchers");
        if (m_isCreated)
        {
            bool errorOccured;
            bool destroyed = destroy(errorOccured);
            ASSERT(!errorOccured, "Resource destruction failed");
        }
	}

    Resource(const Resource&) = delete;
    Resource& operator=(const Resource&) = delete;
    Resource(Resource&&) noexcept = default;
    Resource& operator=(Resource&&) noexcept = default;
private:
	std::string m_name;

	uint32_t m_watchers = 0;
	bool m_isCreated = false;
};

} // Engine

#endif // !RESOURCE_HPP