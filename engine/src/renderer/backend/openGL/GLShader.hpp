#ifndef GL_SHADER_HPP
#define GL_SHADER_HPP

#include "renderer/backend/openGL/GLWrapper.hpp"
#include "resources/Resource.hpp"
#include "resources/ResourceHandle.hpp"
#include "resources/types/TextFileResource.hpp"

namespace Engine {

enum class ShaderType {
	Vertex,
	Fragment
};

class GLShader : public Resource<GLShader> {
	SETUP_ID(Resource, GLShader)
public:
	GLShader();
	GLShader(const std::string& resourceName, ResourceHandle<TextFileResource>&& handle, ShaderType type);

	GLuint getGLID() { return m_glShader; }
	virtual bool isValid() override;

	GLShader(GLShader&&) noexcept = default;
	GLShader& operator=(GLShader&&) noexcept = default;
	GLShader(const GLShader&) = delete;
	GLShader& operator=(const GLShader&) = delete;
private:
	virtual bool createImplementation() override;
	virtual bool destroyImplementation() override;

	ResourceHandle<TextFileResource> m_handle;
	ShaderType m_type;
	GLuint m_glShader;
};

} // Engine

#endif // !GL_SHADER_HPP