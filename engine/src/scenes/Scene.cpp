#include "scenes/Scene.hpp"

#include "events/EventDispatcher.hpp"

namespace Engine {

void Scene::init()
{
	m_systemManager.setEngineContext(getEngineContext());

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