#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <unordered_map>
#include <string>

#include "core/Types.hpp"
#include "utilities/Concepts.hpp"
#include "resources/Resource.hpp"
#include "resources/ResourceHandle.hpp"

namespace Engine {

template<typename ResourceType>
requires 
DerivedFrom<ResourceType, Resource<ResourceType>> && 
DefaultConstructible<ResourceType>
class ResourceManager {
public:
	ResourceHandle<ResourceType> addResource(ResourceType&& resource)
	{
		// Check if the resource name already exists
		const std::string& resourceName = resource.getName();
		if (m_nameToID.find(resourceName) != m_nameToID.end())
		{
			// Return the existing resource handle if the resource is already added
			return getResource(resourceName);
		}

		ID id = resource.getInstanceID();
		m_nameToID[resourceName] = id;
		return m_resources.emplace(id, std::move(resource));
	}

	ResourceHandle<ResourceType> getResource(const std::string& name)
	{
		ID id = getResourceID(name);
		return getResource(id);
	}

	ResourceHandle<ResourceType> getResource(ID id)
	{
		auto it = m_resources.find(id);
		if (it != m_resources.end()) {
			return ResourceHandle<ResourceType>(&(it->second));
		}
		return ResourceHandle<ResourceType>(); // null or invalid handle
	}

	ID getResourceID(const std::string& name)
	{
		auto it = m_nameToID.find(name);
		if (it != m_nameToID.end())
			return it->second;
		return INVALID_ID;
	}
private:
	std::unordered_map<ID, ResourceType> m_resources;
	std::unordered_map<std::string, ID> m_nameToID;
};

} // Engine

#endif // !RESOURCE_MANAGER_HPP