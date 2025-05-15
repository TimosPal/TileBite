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
    bool create()
    {
        if (createImplementation())
        {
            m_isCreated = true;
            return true;
        }
        return false;
    }

    bool destroy()
    {
        if (destroyImplementation())
        {
            m_isCreated = false;
            return true;
        }
        return false;
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
        if (!m_isCreated) return;
        bool destroyed = destroy();
        ASSERT(destroyed, "Resource destruction failed");
	}

    Resource(Resource&& other) noexcept
        : m_name(std::move(other.m_name)),
        m_isCreated(other.m_isCreated)
    {
        // prevent destroy() in moved-from object
        other.m_isCreated = false;
    }

    Resource(const Resource&) = delete;
    Resource& operator=(const Resource&) = delete;
    Resource& operator=(Resource&&) noexcept = delete;
private:
	std::string m_name;
	bool m_isCreated = false;
};

} // Engine

#endif // !RESOURCE_HPP