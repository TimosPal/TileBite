#ifndef RESOURCE_HANDLE_HPP
#define RESOURCE_HANDLE_HPP

#include "utilities/Logger.hpp"
#include "utilities/misc.hpp"
#include "resources/Resource.hpp"

namespace Engine {

// Resources should outlast lifetime of ResourceHandles
// Otherwise this can lead to UB from dangling references.
template<typename ResourceType>
requires DerivedFrom<ResourceType, Resource<ResourceType>>
class ResourceHandle {
public:
	ResourceHandle(ResourceType* resource = nullptr)
		: m_resource(resource)
	{
		if (m_resource) m_resource->watch();
	}

	~ResourceHandle()
	{
		cleanup();
	}

	// Copy constructor
	ResourceHandle(const ResourceHandle& other)
		: m_resource(other.m_resource)
	{
		if (m_resource) m_resource->watch();
	}

	// Move constructor
	ResourceHandle(ResourceHandle&& other) noexcept
		: m_resource(other.m_resource)
	{
		other.m_resource = nullptr;
	}

	// Copy assignment
	ResourceHandle& operator=(const ResourceHandle& other)
	{
		if (this != &other)
		{
			if(m_resource != other.m_resource) cleanup();
			m_resource = other.m_resource;
			if (m_resource) m_resource->watch();
		}
		return *this;
	}

	// Move assignment
	ResourceHandle& operator=(ResourceHandle&& other) noexcept
	{
		if (this != &other)
		{
			if (m_resource != other.m_resource) cleanup();
			m_resource = other.m_resource;
			other.m_resource = nullptr;
		}
		return *this;
	}

	bool isValid() 
	{ 
		if (!m_resource) return false;
		return m_resource->isValid();
	}

	bool isLoaded() const { return m_resource.isCreated(); }

	bool load() {
		if (!m_resource) return false;

		bool errorOccured = false;
		bool created = m_resource->create(errorOccured);
		if (errorOccured)
			LOG_ERROR("Failed to load resource: {}", m_resource->getName());
		if (created)
			LOG_INFO("Loaded resource: {}", m_resource->getName());
		return !errorOccured;
	}

	bool unload() {
		if (!m_resource) return false;

		bool errorOccured = false;
		bool destroyed = m_resource->destroy(errorOccured);
		if (errorOccured)
			LOG_ERROR("Failed to unload resource: {}", m_resource->getName());
		if (destroyed)
			LOG_INFO("Unloaded resource: {}", m_resource->getName());
		return !errorOccured;
	}

	ResourceType* getResource() { return m_resource; }
private:
	ResourceType* m_resource = nullptr;

	void cleanup()
	{
		if (m_resource)
		{
			m_resource->unwatch();
			bool unloaded = unload();
			ASSERT(unloaded, "ResourceHandle cleanup: Failed to destroy resource: {}");
		}
	}
};

} // Engine

#endif // !RESOURCE_HANDLE_HPP