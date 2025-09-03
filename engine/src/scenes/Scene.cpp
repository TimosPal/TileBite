#include "scenes/Scene.hpp"

#include "events/EventDispatcher.hpp"
#include "window/Window.hpp"

namespace TileBite {

void Scene::init()
{
	m_systemManager.setEngineContext(getEngineContext());

	m_world.setRemoveEntityCallback([&](ID entityID) {
		m_physicsEngine.removeCollider(entityID);
		m_sceneGraph.detachFromParent(entityID);
	});

	// TODO: Same thing should happen if the sceneGraph or physicsEngine should be updated
	// from a component removal (for example a collider component removed).
}

void Scene::onUpdate(float deltaTime)
{
	m_systemManager.updateSystems(deltaTime);
}

void Scene::setCameraController(std::shared_ptr<CameraController> cameraController)
{
	m_cameraController = cameraController;

	// Set the initial camera projection based on the window size.
	int width = getWindow().getWidth();
	int height = getWindow().getHeight();
	pushEvent(std::make_unique<WindowResizeEvent>(WindowResizeEvent(width, height)));
}

} // TileBite