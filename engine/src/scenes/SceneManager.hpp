#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include "core/pch.hpp"
#include "scenes/Scene.hpp"

namespace Engine {

class SceneManager {
public:
	SceneManager(AssetsManager* assetsManager) :
		m_assetsManager(assetsManager) {}

	// Create a new scene with the given name.
	template<typename SceneDerived>
	std::shared_ptr<Scene> createScene(const std::string& name)
	{
		auto scene = std::make_shared<SceneDerived>();
		scene->setAssetsManager(m_assetsManager);
		scene->setSceneManager(this);
		scene->setPushEventCallable(m_pushEventCallable);
		m_scenes[name] = scene;
		return scene;
	}

	bool setActiveScene(const std::string& name);
	bool setActiveScene(std::shared_ptr<Scene> scene);
	std::shared_ptr<Scene> getActiveScene() const;

	void setPushEventCallable(std::function<void(std::unique_ptr<Event>)> pushEventCallable)
	{
		m_pushEventCallable = std::move(pushEventCallable);
	}

	void clearScenes() 
	{ 
		m_scenes.clear(); 
		m_activeScene.reset();
	}
private:
	std::shared_ptr<Scene> m_activeScene = nullptr;
	std::unordered_map<std::string, std::shared_ptr<Scene>> m_scenes;

	AssetsManager* m_assetsManager = nullptr;

	std::function<void(std::unique_ptr<Event>)> m_pushEventCallable;
};

} // Engine

#endif // !SCENE_MANAGER_HPP