#include "scenes/SceneManager.hpp"

namespace TileBite {

// Set the active scene by name.
bool SceneManager::setActiveScene(const std::string& name)
{
	auto it = m_scenes.find(name);
	if (it != m_scenes.end())
	{
		return setActiveScene(it->second);
	}
	else
	{
		return false;
	}
}

bool SceneManager::setActiveScene(std::shared_ptr<Scene> scene)
{
	m_activeScene = scene;
	m_activeScene->onLoad();
	return true;
}

// Get the active scene.
std::shared_ptr<Scene> SceneManager::getActiveScene() const
{
	return m_activeScene;
}

} // TileBite