#ifndef ENGINE_COMPONENTS_HPP
#define ENGINE_COMPONENTS_HPP

namespace Engine {

struct SpriteComponent
{
	float r, g, b;
};

struct TransformComponent
{
	float x, y;
	float w, h;
};

} // Engine

#endif // !ENGINE_COMPONENTS_HPP
