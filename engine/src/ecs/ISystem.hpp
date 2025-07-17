#ifndef ISYSTEM_HPP
#define ISYSTEM_HPP

#include "core/pch.hpp"
#include "events/Event.hpp"
#include "core/EngineContext.hpp"

namespace Engine {

class ISystem : public InjectEngineContext {
public:
    virtual ~ISystem() = default;
    virtual void update(float deltaTime) = 0;
private:
};

} // Engine

#endif // !ISYSTEM_HPP
