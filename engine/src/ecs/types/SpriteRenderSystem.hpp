#ifndef SPRITE_RENDER_SYSTEM_HPP
#define SPRITE_RENDER_SYSTEM_HPP

#include "ecs/ISystem.hpp"
#include "ecs/types/EngineComponents.hpp"
#include "renderer/Renderer2D.hpp"

#include "core/EngineApp.hpp"

namespace TileBite {

class SpriteRenderSystem : public ISystem {
public:
	virtual void update(float deltaTime) override
	{
		auto& renderer2D = EngineApp::getInstance()->getRenderer();
		auto activeScene = EngineApp::getInstance()->getSceneManager().getActiveScene();
		auto& activeWorld = activeScene->getWorld();
		auto& activeSceneGraph = activeScene->getSceneGraph();

		World::TypePack<ParentComponent> excludedTypes;

		// Render children without parent link
		activeWorld.query<SpriteComponent, TransformComponent>(excludedTypes).each([&](ID entityID, SpriteComponent* spriteComp, TransformComponent* transformComp) {
			renderer2D.drawQuad(SpriteQuad{ transformComp, spriteComp });
		});

		// Render children with parent link
		activeWorld.query<SpriteComponent, TransformComponent, ParentComponent>().each([&](
			ID entityID, SpriteComponent* spriteComp, 
			TransformComponent* transformComp, ParentComponent* currentLink) 
		{
			auto& worldTransform = activeSceneGraph.getWorldTransform(entityID);
			renderer2D.drawQuad(SpriteQuad{ &worldTransform, spriteComp });
		});
	}
};

} // TileBite

#endif // !SPRITE_RENDER_SYSTEM_HPP
