#ifndef SPRITE_QUAD_HPP
#define SPRITE_QUAD_HPP

#include "utilities/IDGenerator.hpp"

#include "ecs/types/EngineComponents.hpp"
#include "utilities/Bitset.hpp"
#include "resources/types/TilemapResource.hpp"

namespace TileBite {

// TODO: make it support const pointers
struct SpriteQuad {
	TransformComponent* TransformComp;
	SpriteComponent* SpriteComp;
};

struct TilemapMesh {
	TilemapResource* TilemapResource;
	TransformComponent* TransformComp;
};

// TODO: could maybe remove this struct and just use two glm::vec2 in drawLine
struct Line {
	glm::vec2 Start;
	glm::vec2 End;
	glm::vec4 Color;
};

} // TileBite

#endif // !SPRITE_QUAD_HPP