#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include "core/pch.hpp"
#include "core/Types.hpp"
#include "utilities/misc.hpp"
#include "resources/Resource.hpp"
#include "resources/ResourceHandle.hpp"
#include "resources/ControlBlock.hpp"

namespace TileBite {

template<typename ResourceType>
requires 
DerivedFrom<ResourceType, Resource<ResourceType>> && 
DefaultConstructible<ControlBlock<ResourceType>>
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
		ControlBlock<ResourceType> block(std::move(resource));
		auto [it, inserted] = m_resources.emplace(id, std::move(block));
		return ResourceHandle<ResourceType>(&(it->second));
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

	void clear() 
	{
		m_resources.clear();
		m_nameToID.clear();
	}
private:
	std::unordered_map<ID, ControlBlock<ResourceType>> m_resources;
	std::unordered_map<std::string, ID> m_nameToID;
};

} // TileBite

#endif // !RESOURCE_MANAGER_HPP