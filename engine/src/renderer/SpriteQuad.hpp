#ifndef SPRITE_QUAD_HPP
#define SPRITE_QUAD_HPP

namespace Engine {

struct SpriteQuad {
	// Position
	float x, y;
	
	// Size
	float w, h;

	// Tint
	float r, g, b;

	// Quad uvs (Default set for whole texture)
	float u0 = 0.0f;
	float v0 = 0.0f;
	float u1 = 1.0f;
	float v1 = 1.0f;
};

} // Engine

#endif // !SPRITE_QUAD_HPP