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
		world.query<SpriteComponent, TransformComponent>().each([this](SpriteComponent* spriteComp, TransformComponent* transformComp) {
			SpriteQuad spriteQuad
			{
				transformComp->x,
				transformComp->y,
				transformComp->w,
				transformComp->h,
				spriteComp->r,
				spriteComp->g,
				spriteComp->b,
				spriteComp->a
			};
			m_renderer2D->drawQuad(std::move(spriteQuad));
		});
	}

private:
	std::shared_ptr<Renderer2D> m_renderer2D;
};

} // Engine

#endif // !SPRITE_RENDER_SYSTEM_HPP
