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

	// Quad uvs (Default set for whole texture)
	float u0 = 0.0f;
	float v0 = 1.0f;
	float u1 = 1.0f;
	float v1 = 0.0f;

	ID spriteID;
};

} // Engine

#endif // !SPRITE_QUAD_HPP