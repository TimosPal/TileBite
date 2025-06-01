#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include "core/pch.hpp"
#include "scenes/Scene.hpp"

namespace Engine {

class SceneManager {
public:
	std::shared_ptr<Scene> createScene(const std::string& name);
	bool setActiveScene(const std::string& name);
	std::shared_ptr<Scene> getActiveScene() const;
private:
	std::shared_ptr<Scene> m_activeScene;
	std::unordered_map<std::string, std::shared_ptr<Scene>> m_scenes;
};

} // Engine

#endif // !SCENE_MANAGER_HPP