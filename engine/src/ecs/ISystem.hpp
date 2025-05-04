#ifndef ISYSTEM_HPP
#define ISYSTEM_HPP

#include "core/pch.hpp"
#include "ecs/World.hpp"

namespace Engine {

class ISystem {
public:
    virtual ~ISystem() = default;
    virtual void update(World& world, float deltaTime) = 0;
};

} // Engine

#endif // !ISYSTEM_HPP