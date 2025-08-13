#ifndef COLLIDER_RENDER_SYSTEM_HPP
#define COLLIDER_RENDER_SYSTEM_HPP

#include "ecs/ISystem.hpp"
#include "ecs/types/EngineComponents.hpp"
#include "renderer/Renderer2D.hpp"
#include "utilities/misc.hpp"

#include "physics/aabb.hpp"

namespace Engine {

class ColliderRenderSystem : public ISystem {
public:
	virtual void update(float deltaTime) override
	{
		auto& renderer2D = getRenderer();
		const auto& coreTreeBounds = getSceneManager().getActiveScene()->getPhysicsEngine().getCoreTreeInternalBounds();
		for(const auto& bound : coreTreeBounds)
		{
			glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // Red color for bounds
			renderer2D.drawSquare(bound.Min, bound.Max, color);
		}

		const auto& coreTreeColliders = getSceneManager().getActiveScene()->getPhysicsEngine().getCoreTreeColliders();
		for (const auto& collider : coreTreeColliders)
		{
			glm::vec4 color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); // Green color for AABB
			renderer2D.drawSquare(collider.Min, collider.Max, color);
		}

		const auto& tilemapTreeBounds = getSceneManager().getActiveScene()->getPhysicsEngine().getTilemapTreeInternalBounds();
		for (const auto& bound : tilemapTreeBounds)
		{
			glm::vec4 color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // Blue color for AABB
			renderer2D.drawSquare(bound.Min, bound.Max, color);
		}

		const auto& tilemapTreeColliders = getSceneManager().getActiveScene()->getPhysicsEngine().getTilemapTreeColliders();
		for (const auto& collider : tilemapTreeColliders)
		{
			glm::vec4 color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow color for AABB
			renderer2D.drawSquare(collider.Min, collider.Max, color);
		}
	}
};

} // Engine

#endif // !COLLIDER_RENDER_SYSTEM_HPP
