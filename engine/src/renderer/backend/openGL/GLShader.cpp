#include "renderer/backend/openGL/GLShader.hpp"

#include "utilities/Logger.hpp"

namespace Engine {

GLShader::GLShader(const std::string& resourceName, ResourceHandle<TextFileResource>&& handle, ShaderType type)
	: Resource<GLShader>(resourceName),
	m_handle(std::move(handle)),
	m_type(type),
	m_glShader(0)
{}

bool GLShader::createImplementation()
{
	auto glVertexType = (m_type == ShaderType::Vertex) ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
	GL_RET(glCreateShader(glVertexType), m_glShader);

	m_handle.watch();
	bool success = m_handle.load();
	if (!success)
	{
		LOG_ERROR("Error loading source file");
		return false;
	}
	const char* shaderSourceCodeCString = m_handle.getResource()->getData().c_str();
	GL(glShaderSource(m_glShader, 1, &shaderSourceCodeCString, NULL));
	GL(glCompileShader(m_glShader));

	int successIv;
	char infoLog[512];
	GL(glGetShaderiv(m_glShader, GL_COMPILE_STATUS, &successIv));
	if (!successIv)
	{
		GL(glGetShaderInfoLog(m_glShader, 512, NULL, infoLog));
		LOG_ERROR("{} COMPILATION_FAILED: {}", (m_type == ShaderType::Vertex) ? "Vertex" : "Fragment", infoLog);
		return false;
	}

	return true;
}

bool GLShader::destroyImplementation()
{
	GL(glDeleteShader(m_glShader));
	m_glShader = 0;

	m_handle.unwatch();
	m_handle.unload();

	return true;
}

bool GLShader::isValid()
{
	if (!m_handle.isValid())
	{
		LOG_ERROR("Invalid shader handle");
		return false;
	}
	return true;
}

} // Engine