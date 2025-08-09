#ifndef SPRITE_RENDER_SYSTEM_HPP
#define SPRITE_RENDER_SYSTEM_HPP

#include "ecs/ISystem.hpp"
#include "ecs/types/EngineComponents.hpp"
#include "renderer/Renderer2D.hpp"

namespace Engine {

class SpriteRenderSystem : public ISystem {
public:
	virtual void update(float deltaTime) override
	{
		auto& renderer2D = getRenderer();
		auto& activeWorld = getSceneManager().getActiveScene()->getWorld();
		activeWorld.query<SpriteComponent, TransformComponent>().each([&](ID entityID, SpriteComponent* spriteComp, TransformComponent* transformComp) {
			renderer2D.drawQuad(SpriteQuad{ transformComp, spriteComp });
		});
	}
};

} // Engine

#endif // !SPRITE_RENDER_SYSTEM_HPP
