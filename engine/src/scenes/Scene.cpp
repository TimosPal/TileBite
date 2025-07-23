#include "scenes/Scene.hpp"

#include "events/EventDispatcher.hpp"
#include "events/types/EntityEvents.hpp"

namespace Engine {

void Scene::init()
{
	m_systemManager.setAssetsManager(&getAssetsManager());
	m_systemManager.setSceneManager(&getSceneManager());
	m_systemManager.setCoreEventDispatcher(&getCoreEventDispatcher());
	m_systemManager.setPushEventCallable(getPushEventCallable());

	m_world.setPushEvent(getPushEventCallable());

	// Subscribe to ECS change events to update physics colliders
	EventCallback<EntityAddComponentEvent> entAddComponentCallback([&](EntityAddComponentEvent& event) {
		// TODO: getComponents (?)
		ID entityID = event.getEntityID();
		TransformComponent* transformComp = m_world.getComponent<TransformComponent>(entityID);
		AABB* aabb = m_world.getComponent<AABB>(entityID);
		if (!transformComp || !aabb) return;

		m_physicsEngine.addCollider(entityID, aabb, transformComp);
	});
	getCoreEventDispatcher().subscribe(entAddComponentCallback);

	EventCallback<EntityRemoveEvent> entRemoveCallback([&](EntityRemoveEvent& event) {
		ID entityID = event.getEntityID();
		m_physicsEngine.removeCollider(entityID);
	});
	getCoreEventDispatcher().subscribe(entRemoveCallback);

	// TODO: add for remove component if colider was removed.
}

void Scene::onUpdate(float deltaTime)
{
	m_systemManager.updateSystems(deltaTime);
}

} // Engine