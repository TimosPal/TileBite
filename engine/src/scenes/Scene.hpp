#ifndef SCENE_HPP
#define SCENE_HPP

#include "core/pch.hpp"
#include "ecs/World.hpp"
#include "renderer/camera/CameraController.hpp"
#include "ecs/SystemManager.hpp"
#include "physics/PhysicsEngine.hpp"

namespace Engine {

class Scene {
public:
	World& getWorld() { return m_world; }
	PhysicsEngine& getPhysicsEngine() { return m_physicsEngine; } // TODO: maybe return a protected interface instead of the whole physics engine?
	
	std::shared_ptr<CameraController> getCameraController() const { return m_cameraController; }
	
	void onUpdate(float deltaTime) 
	{
		// TODO: really slow to re create colliders each frame
		// (place holder code!!!)
		// Need to add and remove on spawn / despawn of entities with colliders
		m_physicsEngine.clearColliders(); // Clear colliders at the start of each update
		m_world.query<AABB, TransformComponent>().each([this](ID entityID, AABB* aabb, TransformComponent* transformComp) {
			// Draw AABB as a rectangle
			glm::vec2 min = aabb->Min * transformComp->Size + transformComp->Position;
			glm::vec2 max = aabb->Max * transformComp->Size + transformComp->Position;
			m_physicsEngine.addCollider(AABB(min, max), entityID);
		});

		m_systemManager.updateSystems(deltaTime); 
	}
	void updateWorldActions() { m_world.executeDeferredActions(); }

	void setSceneManager(SceneManager* sceneManager) { m_sceneManager = sceneManager; }
	void setAssetsManager(AssetsManager* assets) { m_assetsManager = assets; }
	void setPushEventCallable(std::function<void(std::unique_ptr<Event>)> pushEventCallable)
	{
		m_pushEventCallable = std::move(pushEventCallable);
	}

	virtual void onLoad() {};
protected:

	void setCameraController(std::shared_ptr<CameraController> cameraController) { m_cameraController = cameraController; }
	
	SceneManager* getSceneManager() {
		ASSERT(m_sceneManager != nullptr, "Scene manager not injected");
		return m_sceneManager;
	}

	AssetsManager* getAssetsManager() {
		ASSERT(m_assetsManager != nullptr, "Assets manager not injected");
		return m_assetsManager;
	}

	void addSystem(std::unique_ptr<ISystem> system)
	{
		// Inject world and asset manager for hidden client side use
		// (Removes the need for client side construction to include injections)
		system->setAssetsManager(getAssetsManager());
		system->setSceneManager(getSceneManager());
		system->setPushEventCallable(m_pushEventCallable);

		m_systemManager.addSystem(std::move(system));
	}
private:
	// NOTE: each scene has its own world and own physics colliders.
	World m_world;
	PhysicsEngine m_physicsEngine;

	std::shared_ptr<CameraController> m_cameraController;
	SystemManager m_systemManager;

	SceneManager* m_sceneManager = nullptr;
	AssetsManager* m_assetsManager = nullptr;

	std::function<void(std::unique_ptr<Event>)> m_pushEventCallable;
};

} // Engine

#endif // !SCENE_HPP