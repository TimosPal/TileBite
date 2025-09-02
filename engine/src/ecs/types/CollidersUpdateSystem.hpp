#ifndef COLLIDER_UPDATE_SYSTEM_HPP
#define COLLIDER_UPDATE_SYSTEM_HPP

#include "ecs/ISystem.hpp"
#include "ecs/types/EngineComponents.hpp"
#include "utilities/misc.hpp"

namespace TileBite {

class ColliderUpdateSystem : public ISystem {
public:
    virtual void update(float deltaTime) override {
        auto& physicsEngine = getSceneManager().getActiveScene()->getPhysicsEngine();
        auto& world = getSceneManager().getActiveScene()->getWorld();

        updateColliderType<AABBComponent>(world, physicsEngine);
        updateColliderType<OBBComponent>(world, physicsEngine);
        updateColliderType<CircleColliderComponent>(world, physicsEngine);

        // Tilemaps are special
        world.query<TilemapComponent, TransformComponent>().each([&](ID entityID, TilemapComponent* tilemap, TransformComponent* transform) {
            if (transform->isDirty() || tilemap->isDirty()) {
                physicsEngine.updateTilemapColliderGroup(
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

private:
	template<typename ColliderComponent>
	void updateColliderType(World& world, PhysicsEngine& physicsEngine) {
		World::TypePack<ParentLinkComponent> excludedTypes;
        
        // Update colliders that have no parent link
		world.query<ColliderComponent, TransformComponent>(excludedTypes).each([&](ID entityID, ColliderComponent* collider, TransformComponent* transform) {
			if (transform->isDirty() || collider->isDirty()) {
				physicsEngine.updateCollider(entityID, &collider->getCollider(), transform);
				transform->resetDirty();
				collider->resetDirty();
			}
	    });

        // Update colliders that have no parent link
        world.query<ColliderComponent, TransformComponent, ParentLinkComponent>().each([&](
            ID entityID,
            ColliderComponent* collider,
            TransformComponent* transform,
            ParentLinkComponent* curentLink) 
        {
            if (transform->isDirty() || collider->isDirty()) {
                physicsEngine.updateCollider(entityID, &collider->getCollider(), &curentLink->CachedParentTransform);
                transform->resetDirty();
                collider->resetDirty();
            }
        });
	}

};

} // TileBite

#endif // !COLLIDER_UPDATE_SYSTEM_HPP
