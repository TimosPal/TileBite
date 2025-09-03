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
		auto& activeSceneGraph = getSceneManager().getActiveScene()->getSceneGraph();

        updateColliderType<AABBComponent>(world, physicsEngine, activeSceneGraph);
        updateColliderType<OBBComponent>(world, physicsEngine, activeSceneGraph);
        updateColliderType<CircleColliderComponent>(world, physicsEngine, activeSceneGraph);

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
	void updateColliderType(World& world, PhysicsEngine& physicsEngine, SceneGraph& activeSceneGraph) {
		World::TypePack<ParentComponent> excludedTypes;
        
        // Update colliders that have no parent link
		world.query<ColliderComponent, TransformComponent>(excludedTypes).each([&](ID entityID, ColliderComponent* collider, TransformComponent* transform) {
			if (transform->isDirty() || collider->isDirty()) {
				physicsEngine.updateCollider(entityID, &collider->getCollider(), transform);
				transform->resetDirty();
				collider->resetDirty();
			}
	    });

        // Update colliders that have no parent link
        world.query<ColliderComponent, TransformComponent, ParentComponent>().each([&](
            ID entityID,
            ColliderComponent* collider,
            TransformComponent* transform,
            ParentComponent* curentLink) 
        {
            auto& worldTransform = activeSceneGraph.getWorldTransform(entityID);
            if (collider->isDirty() || worldTransform.isDirty()) {
                physicsEngine.updateCollider(entityID, &collider->getCollider(), &worldTransform);
                transform->resetDirty();
                collider->resetDirty();
				worldTransform.resetDirty();
            }
        });
	}

};

} // TileBite

#endif // !COLLIDER_UPDATE_SYSTEM_HPP
