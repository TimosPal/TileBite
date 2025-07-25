#include "scenes/Scene.hpp"

#include "events/EventDispatcher.hpp"

namespace Engine {

void Scene::init()
{
	m_systemManager.setAssetsManager(&getAssetsManager());
	m_systemManager.setSceneManager(&getSceneManager());
	m_systemManager.setCoreEventDispatcher(&getCoreEventDispatcher());
	m_systemManager.setPushEventCallable(getPushEventCallable());

	m_world.setRemoveEntityCallback([&](ID entityID) {
		m_physicsEngine.removeCollider(entityID);
	});

	// TODO: add for remove component if colider was removed.
}

void Scene::onUpdate(float deltaTime)
{
	// Place holder for colliders update
	m_world.query<AABB, TransformComponent>().each([&](ID entityID, AABB* aabb, TransformComponent* transform) {
		if(transform->isDirty() || aabb->isDirty())
		{
			m_physicsEngine.updateCollider(entityID, aabb, transform);
			transform->resetDirty();
			aabb->resetDirty();
		}
	});

	m_systemManager.updateSystems(deltaTime);
}

} // Engine