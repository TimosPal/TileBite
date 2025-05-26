#ifndef SPRITE_QUAD_HPP
#define SPRITE_QUAD_HPP

#include "utilities/IDGenerator.hpp"

#include "ecs/types/EngineComponents.hpp"

namespace Engine {

struct SpriteQuad {
	TransformComponent* TransformComp;
	SpriteComponent* SpriteComp;
};

} // Engine

#endif // !SPRITE_QUAD_HPP