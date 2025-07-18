#ifndef SCENE_HPP
#define SCENE_HPP

#include "core/pch.hpp"
#include "ecs/World.hpp"
#include "renderer/camera/CameraController.hpp"
#include "ecs/SystemManager.hpp"
#include "physics/PhysicsEngine.hpp"
#include "core/EngineContext.hpp"

namespace Engine {

class Scene : public InjectEngineContext {
public:
	void init()
	{
		m_systemManager.setAssetsManager(&getAssetsManager());
		m_systemManager.setSceneManager(&getSceneManager());
		m_systemManager.setCoreEventDispatcher(&getCoreEventDispatcher());
		m_systemManager.setPushEventCallable(getPushEventCallable());
	}

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

	virtual void onLoad() {};
protected:
	void setCameraController(std::shared_ptr<CameraController> cameraController) { m_cameraController = cameraController; }

	SystemManager& getSystemManager() { return m_systemManager; } // TODO: make a more protective interface for client side use.
private:
	// NOTE: each scene has its own world and own physics colliders.
	World m_world;
	PhysicsEngine m_physicsEngine;

	std::shared_ptr<CameraController> m_cameraController;
	SystemManager m_systemManager;
};

} // Engine

#endif // !SCENE_HPP