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
		glm::vec4 boundsColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // Red color for bounds
		glm::vec4 collidersColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); // Green color for colliders
		glm::vec4 tilemapBoundsColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // Blue color for tilemap bounds
		glm::vec4 tilemapColliderGroupColor = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow color for tilemap group
		
		auto& renderer2D = getRenderer();
		const auto& coreTreeBounds = getSceneManager().getActiveScene()->getPhysicsEngine().getCoreTreeInternalBounds();
		for(const auto& bound : coreTreeBounds)
		{
			renderer2D.drawSquare(bound.Min, bound.Max, boundsColor);
		}

		const auto& coreTreeColliders = getSceneManager().getActiveScene()->getPhysicsEngine().getCoreTreeColliders();
		for (const auto& collider : coreTreeColliders)
		{
			switch (collider.Type)
			{
			case Collider::ColliderType::AABBType:
				renderer2D.drawSquare(collider.AABBCollider.Min, collider.AABBCollider.Max, collidersColor);
				break;
			case Collider::ColliderType::OBBType:
				renderer2D.drawSquare(collider.OBBCollider.getCorners(), collidersColor);
				break;
			default:
				break;
			}
		}

		const auto& tilemapTreeBounds = getSceneManager().getActiveScene()->getPhysicsEngine().getTilemapTreeInternalBounds();
		for (const auto& bound : tilemapTreeBounds)
		{
			renderer2D.drawSquare(bound.Min, bound.Max, tilemapBoundsColor);
		}

		const auto& tilemapTreeColliders = getSceneManager().getActiveScene()->getPhysicsEngine().getTilemapTreeColliders();
		for (const auto& collider : tilemapTreeColliders)
		{
			renderer2D.drawSquare(collider.AABBCollider.Min, collider.AABBCollider.Max, tilemapColliderGroupColor);
		}
	}
};

} // Engine

#endif // !COLLIDER_RENDER_SYSTEM_HPP
