#ifndef GL_RESOURCE_HUB_HPP
#define GL_RESOURCE_HUB_HPP

#include "resources/ResourceManager.hpp"
#include "renderer/backend/openGL/GLProgram.hpp"
#include "renderer/backend/openGL/GLShader.hpp"
#include "resources/types/SystemResourceHub.hpp"

namespace Engine {

class GLResourceHub {
public:
	GLResourceHub(SystemResourceHub& systemResourceHub)
		: m_systemResourceHub(systemResourceHub)
	{}

	bool init();

	template<typename T>
	ResourceManager<T>& getManager();

	template<>
	ResourceManager<GLShader>& getManager<GLShader>() { return m_shadersResourceManager; }

	template<>
	ResourceManager<GLProgram>& getManager<GLProgram>() { return m_programsResourceManager; }
private:
	ResourceManager<GLShader> m_shadersResourceManager;
	ResourceManager<GLProgram> m_programsResourceManager;

	SystemResourceHub& m_systemResourceHub;
};

} // Engine

#endif // !GL_RESOURCE_HUB_HPP
