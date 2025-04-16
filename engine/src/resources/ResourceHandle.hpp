#ifndef RESOURCE_HANDLE_HPP
#define RESOURCE_HANDLE_HPP

#include "utilities/Logger.hpp"
#include "utilities/Concepts.hpp"
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
			m_resource->destroy();
		}
	}

	bool isValid() const { return m_resource != nullptr; }

	bool load()
	{
		if (m_resource)
		{
			bool res = m_resource->create();
			if (!res)
			{
				LOG_ERROR("Failed to load resource: {}", m_resource.getName());
			}
			return res;
		}
		else
		{
			return false;
		}
	}

	void unload()
	{
		if (m_resource)
		{
			m_resource->destroy();
		}
	}

	ResourceType* getResource() { return m_resource; }
private:
	ResourceType* m_resource = nullptr;
};

} // Engine

#endif // !RESOURCE_HANDLE_HPP