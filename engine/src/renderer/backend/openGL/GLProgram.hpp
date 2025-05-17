#ifndef GL_PROGRAM_HPP
#define GL_PROGRAM_HPP

#include "resources/Resource.hpp"
#include "resources/ResourceHandle.hpp"
#include "renderer/backend/openGL/GLShader.hpp"

namespace Engine {

class GLProgram : public Resource<GLProgram> {
	SETUP_ID(Resource, GLShader)
public:
	GLProgram(
		const std::string& resourceName,
		ResourceHandle<GLShader>&& vertexHandle,
		ResourceHandle<GLShader>&& fragmentHandle
	);

	void use();
	void unuse();
	virtual bool isValid() override;

	unsigned int getGLID() { return m_glProgram; }

	GLProgram(GLProgram&&) noexcept = default;
	GLProgram& operator=(GLProgram&&) noexcept = default;
	GLProgram(const GLProgram&) = delete;
	GLProgram& operator=(const GLProgram&) = delete;
private:
	virtual bool createImplementation() override;
	virtual bool destroyImplementation() override;

	ResourceHandle<GLShader> m_vertexHandle;
	ResourceHandle<GLShader> m_fragmentHandle;
	unsigned int m_glProgram = 0;
};

} // Engine

#endif // !GL_PROGRAM_HPP