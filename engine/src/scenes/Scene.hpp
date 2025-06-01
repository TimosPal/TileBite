#ifndef SCENE_HPP
#define SCENE_HPP

#include "core/pch.hpp"
#include "ecs/World.hpp"
#include "renderer/camera/CameraController.hpp"

namespace Engine {

class Scene {
public:
	World& getWorld() { return m_world; }
	std::shared_ptr<CameraController> getCameraController() const { return m_cameraController; }
	void setCameraController(std::shared_ptr<CameraController> cameraController) { m_cameraController = cameraController; }
private:
	World m_world;
	std::shared_ptr<CameraController> m_cameraController;
};

} // Engine

#endif // !SCENE_HPP