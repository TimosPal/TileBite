#ifndef COLLIDER_RENDER_SYSTEM_HPP
#define COLLIDER_RENDER_SYSTEM_HPP

#include "ecs/ISystem.hpp"
#include "ecs/types/EngineComponents.hpp"
#include "renderer/Renderer2D.hpp"
#include "utilities/misc.hpp"

#include "physics/AABB.hpp"

#include "core/EngineApp.hpp"

namespace TileBite {

class ColliderRenderSystem : public ISystem {
public:
	virtual void update(float deltaTime) override
	{
		glm::vec4 boundsColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // Red color for bounds
		glm::vec4 collidersColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); // Green color for colliders
		glm::vec4 tilemapBoundsColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // Blue color for tilemap bounds
		glm::vec4 helperBoundingBoxColor = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow color for bounding box of more complex shapes
		
		auto& renderer2D = EngineApp::getInstance()->getRenderer();
		auto& physicsEngine = EngineApp::getInstance()->getSceneManager().getActiveScene()->getPhysicsEngine();
		const auto& coreTreeBounds = physicsEngine.getCoreTreeInternalBounds();
		for(const auto& bound : coreTreeBounds)
		{
			renderer2D.drawSquare(bound.Min, bound.Max, boundsColor);
		}

		const auto& coreTreeColliders = physicsEngine.getCoreTreeColliders();
		for (const auto& collider : coreTreeColliders)
		{
			switch (collider.Type)
			{
			case Collider::ColliderType::AABB:
				renderer2D.drawSquare(collider.AABBCollider.Min, collider.AABBCollider.Max, collidersColor);
				break;
			case Collider::ColliderType::OBB: {
				renderer2D.drawSquare(collider.OBBCollider.getCorners(), collidersColor);

				AABB boundingBox = collider.OBBCollider.getBoundingBox();
				renderer2D.drawSquare(boundingBox.Min, boundingBox.Max, helperBoundingBoxColor);
				break;
			}
			case Collider::ColliderType::Circle: {
				renderer2D.drawCircle(collider.CircleCollider.Center, collider.CircleCollider.Radius, collidersColor);

				AABB boundingBox = collider.CircleCollider.getBoundingBox();
				renderer2D.drawSquare(boundingBox.Min, boundingBox.Max, helperBoundingBoxColor);
				break;
			}
			default:
				ASSERT_FALSE("Unknown collider type");
				break;
			}
		}

		const auto& tilemapTreeBounds = physicsEngine.getTilemapTreeInternalBounds();
		for (const auto& bound : tilemapTreeBounds)
		{
			renderer2D.drawSquare(bound.Min, bound.Max, tilemapBoundsColor);
		}

		const auto& tilemapTreeColliders = physicsEngine.getTilemapTreeColliders();
		for (const auto& collider : tilemapTreeColliders)
		{
			renderer2D.drawSquare(collider.AABBCollider.Min, collider.AABBCollider.Max, helperBoundingBoxColor);
		}
	}
};

} // TileBite

#endif // !COLLIDER_RENDER_SYSTEM_HPP
