#ifndef COLLIDER_UPDATE_SYSTEM_HPP
#define COLLIDER_UPDATE_SYSTEM_HPP

#include "ecs/ISystem.hpp"
#include "ecs/types/EngineComponents.hpp"
#include "utilities/misc.hpp"

namespace TileBite {

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
				m_physicsEngine.updateCollider(entityID, &aabb->getCollider(), transform);
				transform->resetDirty();
				aabb->resetDirty();
			}
		});

		m_world.query<OBBComponent, TransformComponent>().each([&](ID entityID, OBBComponent* obb, TransformComponent* transform) {
			if (transform->isDirty() || obb->isDirty())
			{
				m_physicsEngine.updateCollider(entityID, &obb->getCollider(), transform);
				transform->resetDirty();
				obb->resetDirty();
			}
		});

		m_world.query<TilemapComponent, TransformComponent>().each([&](ID entityID, TilemapComponent* tilemap, TransformComponent* transform) {
			if (transform->isDirty() || tilemap->isDirty())
			{
				m_physicsEngine.updateTilemapColliderGroup(
					entityID,
					transform,
					glm::vec2(tilemap->getResource()->getWidth(), tilemap->getResource()->getHeight()),
					tilemap->getResource()->getWorldTileSize(),
					tilemap->getResource()->getSolidTiles()
				);
				transform->resetDirty();
				tilemap->resetDirty();
			}
		});
	}
};

} // TileBite

#endif // !COLLIDER_UPDATE_SYSTEM_HPP
