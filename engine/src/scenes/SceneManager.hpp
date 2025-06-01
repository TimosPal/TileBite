#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include "core/pch.hpp"
#include "scenes/Scene.hpp"

namespace Engine {

class SceneManager {
public:
	SceneManager(AssetsManager* assetsManager) :
		m_assetsManager(assetsManager) {}

	std::shared_ptr<Scene> createScene(const std::string& name);
	bool setActiveScene(const std::string& name);
	std::shared_ptr<Scene> getActiveScene() const;

	void setPushEventCallable(std::function<void(std::unique_ptr<Event>)> pushEventCallable)
	{
		m_pushEventCallable = std::move(pushEventCallable);
	}
private:
	std::shared_ptr<Scene> m_activeScene = nullptr;
	std::unordered_map<std::string, std::shared_ptr<Scene>> m_scenes;

	AssetsManager* m_assetsManager = nullptr;

	std::function<void(std::unique_ptr<Event>)> m_pushEventCallable;
};

} // Engine

#endif // !SCENE_MANAGER_HPP