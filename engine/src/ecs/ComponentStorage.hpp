#ifndef COMPONENT_STORAGE_HPP
#define COMPONENT_STORAGE_HPP

#include "core/pch.hpp"

namespace Engine {

class ComponentStorage {
public:
    template<typename T>
    void add(const T& element)
    {
        ASSERT(std::is_trivially_copyable_v<T>, "Only safe for trivially copyable types");
        size_t offset = m_data.size();
        m_data.resize(offset + sizeof(T));
        std::memcpy(m_data.data() + offset, &element, sizeof(T));
    }

    template <typename T>
    T* get(size_t index)
    {
        return reinterpret_cast<T*>(m_data.data() + index * sizeof(T));
    }
private:
    std::vector<std::byte> m_data;
};

} // Engine

#endif // !COMPONENT_STORAGE_HPP