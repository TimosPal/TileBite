#ifndef COLLIDER_UPDATE_SYSTEM_HPP
#define COLLIDER_UPDATE_SYSTEM_HPP

#include "ecs/ISystem.hpp"
#include "ecs/types/EngineComponents.hpp"
#include "utilities/misc.hpp"

namespace Engine {

class ColliderUpdateSystem : public ISystem {
public:
	virtual void update(float deltaTime) override
	{
		// Get the physics engine from the active scene
		auto& m_physicsEngine = getSceneManager().getActiveScene()->getPhysicsEngine();
		auto& m_world = getSceneManager().getActiveScene()->getWorld();
		m_world.query<AABBComponent,TransformComponent>().each([&](ID entityID, AABBComponent* aabb, TransformComponent* transform) {
			if (transform->isDirty() || aabb->isDirty())
			{
				m_physicsEngine.updateCollider(entityID, &aabb->Collider, transform);
				transform->resetDirty();
				aabb->resetDirty();
			}
		});

		m_world.query<TilemapComponent, TransformComponent>().each([&](ID entityID, TilemapComponent* tilemap, TransformComponent* transform) {
			if (transform->isDirty() || tilemap->isDirty())
			{
				m_physicsEngine.updateTilemapColliderGroup(
					entityID,
					transform,
					glm::vec2(tilemap->TilemapResourcePtr->getWidth(), tilemap->TilemapResourcePtr->getHeight()),
					glm::vec2(tilemap->TilemapResourcePtr->getWorldTileSize().x, tilemap->TilemapResourcePtr->getWorldTileSize().y)
				);
				transform->resetDirty();
				tilemap->resetDirty();
			}
		});
	}
};

} // Engine

#endif // !COLLIDER_UPDATE_SYSTEM_HPP
