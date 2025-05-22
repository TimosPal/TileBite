#ifndef ENGINE_COMPONENTS_HPP
#define ENGINE_COMPONENTS_HPP

namespace Engine {

struct SpriteComponent
{
	float r, g, b, a;

	ID textureID = 0;

	// Quad uvs (Default set for whole texture)
	float u0 = 0.0f;
	float v0 = 1.0f;
	float u1 = 1.0f;
	float v1 = 0.0f;
};

struct TransformComponent
{
	float x, y;
	float w, h;
};

} // Engine

#endif // !ENGINE_COMPONENTS_HPP
