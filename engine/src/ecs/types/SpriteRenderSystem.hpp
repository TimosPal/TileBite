#ifndef SPRITE_RENDER_SYSTEM_HPP
#define SPRITE_RENDER_SYSTEM_HPP

#include "ecs/ISystem.hpp"
#include "ecs/types/EngineComponents.hpp"
#include "renderer/Renderer2D.hpp"

namespace Engine {

class SpriteRenderSystem : public ISystem {
public:
	SpriteRenderSystem(std::shared_ptr<Renderer2D> renderer2D) : m_renderer2D(renderer2D) {}

	virtual void update(World& world, float deltaTime) override
	{
		for (auto [spriteComp, transformComp] : world.query<SpriteComponent, TransformComponent>())
		{
			// Render the sprite using the transform
			// This is a placeholder for actual rendering logic
			LOG_INFO("Rendering sprite");
		}
	}

private:
	std::shared_ptr<Renderer2D> m_renderer2D;
};

} // Engine

#endif // !SPRITE_RENDER_SYSTEM_HPP
