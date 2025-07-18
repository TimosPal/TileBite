#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include "core/pch.hpp"
#include "scenes/Scene.hpp"
#include "core/EngineContext.hpp"

namespace Engine {

class SceneManager : public InjectEngineContext {
public:
	// Create a new scene with the given name.
	template<typename SceneDerived>
	std::shared_ptr<Scene> createScene(const std::string& name)
	{
		auto scene = std::make_shared<SceneDerived>();
		scene->setAssetsManager(&getAssetsManager());
		scene->setSceneManager(this);
		scene->setPushEventCallable(getPushEventCallable());
		scene->setCoreEventDispatcher(&getCoreEventDispatcher());
		scene->init();
		m_scenes[name] = scene;
		return scene;
	}

	bool setActiveScene(const std::string& name);
	bool setActiveScene(std::shared_ptr<Scene> scene);
	std::shared_ptr<Scene> getActiveScene() const;

	void clearScenes() 
	{ 
		m_scenes.clear(); 
		m_activeScene.reset();
	}
private:
	std::shared_ptr<Scene> m_activeScene = nullptr;
	std::unordered_map<std::string, std::shared_ptr<Scene>> m_scenes;
};

} // Engine

#endif // !SCENE_MANAGER_HPP