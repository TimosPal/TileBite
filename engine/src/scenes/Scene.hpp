#ifndef SCENE_HPP
#define SCENE_HPP

#include "core/pch.hpp"
#include "ecs/World.hpp"
#include "renderer/camera/CameraController.hpp"
#include "ecs/SystemManager.hpp"

namespace Engine {

class Scene {
public:
	World& getWorld() { return m_world; }
	
	std::shared_ptr<CameraController> getCameraController() const { return m_cameraController; }
	void setCameraController(std::shared_ptr<CameraController> cameraController) { m_cameraController = cameraController; }
	
	void onUpdate(float deltaTime) { m_systemManager.updateSystems(deltaTime); }
	void updateWorldActions() { m_world.executeDeferredActions(); }

	void addSystem(std::unique_ptr<ISystem> system)
	{
		// Inject world and asset manager for hidden client side use
		// (Removes the need for client side construction to include injections)
		system->setAssetsManager(getAssetsManager());
		system->setSceneManager(getSceneManager());
		system->setPushEventCallable(m_pushEventCallable);

		m_systemManager.addSystem(std::move(system));
	}

	SceneManager* getSceneManager() {
		ASSERT(m_sceneManager != nullptr, "Scene manager not injected");
		return m_sceneManager;
	}

	AssetsManager* getAssetsManager() {
		ASSERT(m_assetsManager != nullptr, "Assets manager not injected");
		return m_assetsManager;
	}

	void setSceneManager(SceneManager* sceneManager) { m_sceneManager = sceneManager; }
	void setAssetsManager(AssetsManager* assets) { m_assetsManager = assets; }
	void setPushEventCallable(std::function<void(std::unique_ptr<Event>)> pushEventCallable)
	{
		m_pushEventCallable = std::move(pushEventCallable);
	}
private:
	World m_world;
	std::shared_ptr<CameraController> m_cameraController;
	SystemManager m_systemManager;

	SceneManager* m_sceneManager = nullptr;
	AssetsManager* m_assetsManager = nullptr;

	std::function<void(std::unique_ptr<Event>)> m_pushEventCallable;
};

} // Engine

#endif // !SCENE_HPP