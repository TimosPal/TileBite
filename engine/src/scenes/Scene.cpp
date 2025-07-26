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
	m_systemManager.updateSystems(deltaTime);
}

} // Engine