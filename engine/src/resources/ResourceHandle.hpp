#ifndef RESOURCE_HANDLE_HPP
#define RESOURCE_HANDLE_HPP

#include "utilities/Logger.hpp"
#include "utilities/misc.hpp"
#include "resources/Resource.hpp"
#include "resources/ControlBlock.hpp"

namespace Engine {

// Resources should outlast lifetime of ResourceHandles
// Otherwise this can lead to UB from dangling references.
template<typename ResourceType>
requires DerivedFrom<ResourceType, Resource<ResourceType>>
class ResourceHandle {
public:
	ResourceHandle(ControlBlock<ResourceType> * controlBlock = nullptr)
		: m_controlBlock(controlBlock)
	{}

	// Copy constructor
	ResourceHandle(const ResourceHandle& other)
		: m_controlBlock(other.m_controlBlock)
	{
		if (other.m_isWatching) watch();
	}

	// Move constructor
	ResourceHandle(ResourceHandle&& other) noexcept
		: m_controlBlock(other.m_controlBlock), m_isWatching(other.m_isWatching)
	{
		other.m_controlBlock = nullptr;
		other.m_isWatching = false;
	}

	// Copy assignment
	ResourceHandle& operator=(const ResourceHandle& other)
	{
		if (this != &other)
		{
			if(m_controlBlock != other.m_controlBlock) cleanup();
			m_controlBlock = other.m_controlBlock;
			if (other.m_isWatching) watch();
		}
		return *this;
	}

	// Move assignment
	ResourceHandle& operator=(ResourceHandle&& other) noexcept
	{
		if (this != &other)
		{
			if (m_controlBlock != other.m_controlBlock) cleanup();
			m_controlBlock = other.m_controlBlock;
			if (other.m_isWatching) watch();
			other.m_controlBlock = nullptr;
			other.m_isWatching = false;
		}
		return *this;
	}

	~ResourceHandle()
	{
		cleanup();
	}

	void watch()
	{
		if (m_controlBlock && !m_isWatching)
		{
			m_controlBlock->watch();
			m_isWatching = true;
		}
	}

	void unwatch()
	{
		if (m_controlBlock && m_isWatching)
		{
			m_controlBlock->unwatch();
			m_isWatching = false;
		}
	}

	bool isValid() 
	{ 
		if (!m_controlBlock) return false;
		return m_controlBlock->getResource()->isValid();
	}

	bool isLoaded() const { 
		if (!m_controlBlock) return false;
		return m_controlBlock->getResource()->isCreated();
	}

	// Returns false if error occured.
	bool load() {
		if (!m_controlBlock) return false;

		ResourceType* resRef = m_controlBlock->getResource();
		if (resRef->isCreated()) return true;

		if (resRef->create())
		{
			LOG_INFO("Loaded resource: {}", resRef->getName());
			return true;
		}

		LOG_ERROR("Failed to load resource: {}", resRef->getName());
		return false;
	}

	// Returns false if error occured.
	// if the resource was decided it should not unload still returns true.
	bool unload() {
		if (!m_controlBlock) return false;
		if (m_controlBlock->getWatchCount() > 0) return true;

		ResourceType* resRef = m_controlBlock->getResource();
		if (!resRef->isCreated()) return true;

		if (resRef->destroy())
		{
			LOG_INFO("Unloaded resource: {}", resRef->getName());
			return true;
		}
		
		LOG_ERROR("Failed to unload resource: {}", resRef->getName());
		return false;
	}

	ResourceType* getResource() 
	{ 
		if (!m_controlBlock) return nullptr;
		return m_controlBlock->getResource(); 
	}
private:
	ControlBlock<ResourceType> * m_controlBlock = nullptr;
	bool m_isWatching = false;

	void cleanup()
	{
		if (m_controlBlock)
		{
			unwatch();
			bool success = unload();
			ASSERT(success, "ResourceHandle cleanup: Failed to unload resource: {}");
		}
	}
};

} // Engine

#endif // !RESOURCE_HANDLE_HPP