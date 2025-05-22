#ifndef SPRITE_QUAD_HPP
#define SPRITE_QUAD_HPP

#include "utilities/IDGenerator.hpp"

namespace Engine {

struct SpriteQuad {
	// Position
	float x, y;
	
	// Size
	float w, h;

	// Tint
	float r, g, b, a;

	// Quad uvs 
	float u0, v0, u1, v1;

	ID spriteID;
};

} // Engine

#endif // !SPRITE_QUAD_HPP