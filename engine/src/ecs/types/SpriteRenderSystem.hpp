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
			m_renderer2D->drawQuad(SpriteQuad{ transformComp, spriteComp });
		});
	}

private:
	std::shared_ptr<Renderer2D> m_renderer2D;
};

} // Engine

#endif // !SPRITE_RENDER_SYSTEM_HPP
