#include "scenes/SceneManager.hpp"

namespace Engine {

// Create a new scene with the given name.
std::shared_ptr<Scene> SceneManager::createScene(const std::string& name)
{
	auto scene = std::make_shared<Scene>();
	scene->setAssetsManager(m_assetsManager);
	scene->setSceneManager(this);
	m_scenes[name] = scene;
	return scene;
}

// Set the active scene by name.
bool SceneManager::setActiveScene(const std::string& name)
{
	auto it = m_scenes.find(name);
	if (it != m_scenes.end())
	{
		m_activeScene = it->second;
		return true;
	}
	else
	{
		return false;
	}
}

// Get the active scene.
std::shared_ptr<Scene> SceneManager::getActiveScene() const
{
	return m_activeScene;
}

} // Engine