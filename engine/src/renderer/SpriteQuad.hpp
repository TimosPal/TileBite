#ifndef SPRITE_QUAD_HPP
#define SPRITE_QUAD_HPP

#include "utilities/IDGenerator.hpp"

#include "ecs/types/EngineComponents.hpp"
#include "utilities/Bitset.hpp"

namespace Engine {

struct SpriteQuad {
	TransformComponent* TransformComp;
	SpriteComponent* SpriteComp;
};

struct QuadMesh {
	std::vector<uint32_t>* Vertices;
	ID AtlasID;
};

} // Engine

#endif // !SPRITE_QUAD_HPP