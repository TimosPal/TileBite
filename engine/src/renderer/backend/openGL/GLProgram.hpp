#ifndef GL_PROGRAM_HPP
#define GL_PROGRAM_HPP

#include <glm/mat3x3.hpp>

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

	void setUniform(const std::string& name, int* values, size_t count);
	void setUniform(const std::string& name, const glm::mat4& matrix);
	void setUniform(const std::string& name, float value);

	GLProgram(GLProgram&&) noexcept = default;
	GLProgram& operator=(GLProgram&&) noexcept = default;
	GLProgram(const GLProgram&) = delete;
	GLProgram& operator=(const GLProgram&) = delete;
private:
	virtual bool createImplementation() override;
	virtual bool destroyImplementation() override;

	GLint getUniformLocation(const std::string& name);

	ResourceHandle<GLShader> m_vertexHandle;
	ResourceHandle<GLShader> m_fragmentHandle;
	unsigned int m_glProgram = 0;

	std::unordered_map<std::string, GLint> m_cachedUniformLocations;
};

} // Engine

#endif // !GL_PROGRAM_HPP