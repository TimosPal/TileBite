#include "renderer/backend/openGL/GLShader.hpp"

#include "utilities/Logger.hpp"

namespace Engine {

GLShader::GLShader()
	: Resource<GLShader>(), m_handle(), m_type(ShaderType::Vertex), m_glShader(0)
{}

GLShader::GLShader(const std::string& resourceName, ResourceHandle<TextFileResource>&& handle, ShaderType type)
	: Resource<GLShader>(resourceName), m_handle(std::move(handle)), m_type(type), m_glShader(0)
{}

bool GLShader::createImplementation()
{
	auto glVertexType = (m_type == ShaderType::Vertex) ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
	GL_RET(glCreateShader(glVertexType), m_glShader);

	bool resourceLoaded = m_handle.load();
	if (!resourceLoaded)
	{
		return false;
	}
	const char* shaderSourceCodeCString = m_handle.getResource()->getData().c_str();
	GL(glShaderSource(m_glShader, 1, &shaderSourceCodeCString, NULL));
	GL(glCompileShader(m_glShader));

	int success;
	char infoLog[512];
	GL(glGetShaderiv(m_glShader, GL_COMPILE_STATUS, &success));
	if (!success)
	{
		GL(glGetShaderInfoLog(m_glShader, 512, NULL, infoLog));
		LOG_CRITICAL("{} COMPILATION_FAILED: {}", (m_type == ShaderType::Vertex) ? "Vertex" : "Fragment", infoLog);
		return false;
	}

	return true;
}

bool GLShader::destroyImplementation()
{
	GL(glDeleteShader(m_glShader));
	m_glShader = 0;
	return true;
}

bool GLShader::isValid()
{
	return m_handle.isValid();
}

} // Engine