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
		if (m_resource)
		{
			m_resource->watch();
		}
	}

	~ResourceHandle()
	{
		if (m_resource)
		{
			m_resource->unwatch();
			bool errorOccured;
			m_resource->destroy(errorOccured);
			ASSERT(!errorOccured, "ResourceHandle destructor: Failed to destroy resource: {}");
		}
	}

	bool isValid() const { return m_resource != nullptr; }

	bool load()
	{
		if (m_resource)
		{
			bool errorOccured;
			bool created = m_resource->create(errorOccured);
			if (errorOccured)
			{
				LOG_ERROR("Failed to load resource: {}", m_resource->getName());
			}
			if (created)
			{
				LOG_ERROR("Loaded resource: {}", m_resource->getName());
			}	

			return created;
		}
		else
		{
			return false;
		}
	}

	bool unload()
	{
		if (m_resource)
		{
			bool errorOccured;
			bool destroyed = m_resource->destroy(errorOccured);
			if (errorOccured)
			{
				LOG_ERROR("Failed to unload resource: {}", m_resource->getName());
			}
			if (destroyed)
			{
				LOG_ERROR("Unloaded resource: {}", m_resource->getName());
			}

			return destroyed;
		}
		else
		{
			return false;
		}
	}

	ResourceType* getResource() { return m_resource; }
private:
	ResourceType* m_resource = nullptr;
};

} // Engine

#endif // !RESOURCE_HANDLE_HPP