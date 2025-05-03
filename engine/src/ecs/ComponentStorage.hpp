#ifndef COMPONENT_STORAGE_HPP
#define COMPONENT_STORAGE_HPP

#include "core/pch.hpp"

namespace Engine {

class ComponentStorage {
public:
    ComponentStorage(size_t elementSize);

    void add(const void* element);
    void* get(size_t index);
    void remove(size_t index);

    class Iterator {
    public:
        Iterator(std::vector<std::byte>::iterator it, size_t elementSize)
            : m_it(it), m_elementSize(elementSize)
        {}

        void* operator*() { return static_cast<void*>(&*m_it); }

        Iterator& operator++()
        {
            m_it += m_elementSize;
            return *this;
        }

        bool operator!=(const Iterator& other) const
        {
            return m_it != other.m_it;
        }

    private:
        std::vector<std::byte>::iterator m_it;
        size_t m_elementSize;
    };

    Iterator begin() { return Iterator(m_data.begin(), m_elementSize); }
    Iterator end() { return Iterator(m_data.end(), m_elementSize); }

private:
    size_t m_elementSize;
    std::vector<std::byte> m_data;
};

} // Engine

#endif // !COMPONENT_STORAGE_HPP
