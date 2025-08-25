#ifndef ISYSTEM_HPP
#define ISYSTEM_HPP

#include "core/pch.hpp"
#include "events/Event.hpp"
#include "core/EngineContext.hpp"

namespace TileBite {

class ISystem : public InjectEngineContext {
public:
    virtual ~ISystem() = default;
	virtual void onAttach() {}; // NOTE: called only on creation, consider if this is correct order of operations.
    virtual void update(float deltaTime) {};
private:
};

} // TileBite

#endif // !ISYSTEM_HPP
