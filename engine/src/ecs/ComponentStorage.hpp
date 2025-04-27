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

private:
    size_t m_elementSize;
    std::vector<std::byte> m_data;
};

} // Engine

#endif // !COMPONENT_STORAGE_HPP
