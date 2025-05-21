#ifndef ISYSTEM_HPP
#define ISYSTEM_HPP

#include "core/pch.hpp"
#include "ecs/World.hpp"
#include "resources/AssetsManager.hpp"

namespace Engine {

class ISystem {
public:
    virtual ~ISystem() = default;
    virtual void update(World& world, AssetsManager& assetsManager, float deltaTime) = 0;
};

} // Engine

#endif // !ISYSTEM_HPP