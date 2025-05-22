#ifndef SPRITE_RENDER_SYSTEM_HPP
#define SPRITE_RENDER_SYSTEM_HPP

#include "ecs/ISystem.hpp"
#include "ecs/types/EngineComponents.hpp"
#include "renderer/Renderer2D.hpp"

namespace Engine {

class SpriteRenderSystem : public ISystem {
public:
	SpriteRenderSystem(std::shared_ptr<Renderer2D> renderer2D) : m_renderer2D(renderer2D) {}

	virtual void update(float deltaTime) override
	{
		getWorld()->query<SpriteComponent, TransformComponent>().each([this](SpriteComponent* spriteComp, TransformComponent* transformComp) {
			SpriteQuad spriteQuad
			{
				transformComp->x,
				transformComp->y,
				transformComp->w,
				transformComp->h,
				spriteComp->r,
				spriteComp->g,
				spriteComp->b,
				spriteComp->a,
				spriteComp->u0,
				spriteComp->v0,
				spriteComp->u1,
				spriteComp->v1,
				spriteComp->textureID
			};
			//LOG_INFO("{} {} {} {} {} {} {} {} {}", spriteComp->textureID, spriteComp->r, spriteComp->g, spriteComp->b, spriteComp->a, spriteComp->u0, spriteComp->u1, spriteComp->v0, spriteComp->v1);
			m_renderer2D->drawQuad(std::move(spriteQuad));
		});
	}

private:
	std::shared_ptr<Renderer2D> m_renderer2D;
};

} // Engine

#endif // !SPRITE_RENDER_SYSTEM_HPP
