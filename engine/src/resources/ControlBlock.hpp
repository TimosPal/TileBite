#ifndef CONTROL_BLOCK_HPP
#define CONTROL_BLOCK_HPP

namespace Engine {

// Must be defautl constructible
template<typename ResourceType>
class ControlBlock {
public:
	ControlBlock() 
		: m_resource(nullptr), m_watchers(0)
	{}

	ControlBlock(ResourceType&& resource)
		: m_resource(std::make_unique<ResourceType>(std::move(resource)))
	{}

	~ControlBlock()
	{
		ASSERT(m_watchers >= 0, "ControlBlock destroyed with active watchers");
	}

	ControlBlock(ControlBlock&&) noexcept = default;
	ControlBlock& operator=(ControlBlock&&) noexcept = default;

	ControlBlock(const ControlBlock&) = delete;
	ControlBlock& operator=(const ControlBlock&) = delete;

	void watch() { ++m_watchers; }

	void unwatch()
	{
		--m_watchers;
		ASSERT(m_watchers >= 0, "Watchers below 0");
	}

	int getWatchCount() const { return m_watchers; }

	ResourceType* getResource() { return m_resource.get(); }

private:
	std::unique_ptr<ResourceType> m_resource;
	int m_watchers = 0;
};

} // Engine

#endif // !CONTROL_BLOCK_HPP