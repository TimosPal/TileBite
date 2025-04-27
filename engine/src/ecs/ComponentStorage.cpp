#include "ecs/ComponentStorage.hpp"

#include "utilities/assertions.hpp"

namespace Engine {

    ComponentStorage::ComponentStorage(size_t elementSize) : m_elementSize(elementSize) {}

    void ComponentStorage::add(const void* element)
    {
        size_t offset = m_data.size();
        m_data.resize(offset + m_elementSize);
        std::memcpy(m_data.data() + offset, element, m_elementSize);
    }

    void* ComponentStorage::get(size_t index)
    {
        return reinterpret_cast<void*>(m_data.data() + index * m_elementSize);
    }

    void ComponentStorage::remove(size_t index)
    {
        size_t offset = index * m_elementSize;

        // Ensure we are within bounds
        ASSERT(offset < m_data.size(), "Out of bounds index");

        // Swap the element at index with the last element
        size_t lastElementOffset = m_data.size() - m_elementSize;
        if (offset != lastElementOffset)
        {
            std::memcpy(m_data.data() + offset, m_data.data() + lastElementOffset, m_elementSize);
        }

        // Resize the vector to remove the last element
        m_data.resize(m_data.size() - m_elementSize);
    }


} // Engine