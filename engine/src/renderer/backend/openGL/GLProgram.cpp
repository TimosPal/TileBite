#include "renderer/backend/openGL/GLProgram.hpp"

#include "renderer/backend/openGL/GLWrapper.hpp"
#include "utilities/Logger.hpp"

namespace Engine {

GLProgram::GLProgram(
	const std::string& resourceName,
	ResourceHandle<GLShader>&& vertexHandle,
	ResourceHandle<GLShader>&& fragmentHandle
) 
	: Resource<GLProgram>(resourceName),
	m_vertexHandle(std::move(vertexHandle)),
	m_fragmentHandle(std::move(fragmentHandle))
{}

bool GLProgram::createImplementation()
{
	bool success;
	m_vertexHandle.watch();
	success = m_vertexHandle.load();
	if (!success)
	{
		LOG_ERROR("Error loading vertex shader");
		return false;
	}

	m_fragmentHandle.watch();
	success = m_fragmentHandle.load();
	if (!success)
	{
		LOG_ERROR("Error loading fragment shader");
		return false;
	}

	GL_RET(glCreateProgram(), m_glProgram);

	unsigned int vertexShader = m_vertexHandle.getResource()->getGLID();
	unsigned int fragmentShader = m_fragmentHandle.getResource()->getGLID();

	GL(glAttachShader(m_glProgram, vertexShader));
	GL(glAttachShader(m_glProgram, fragmentShader));
	GL(glLinkProgram(m_glProgram));

	int successIv;
	char infoLog[512];
	GL(glGetProgramiv(m_glProgram, GL_LINK_STATUS, &successIv));
	if (!successIv)
	{
		GL(glGetProgramInfoLog(m_glProgram, 512, NULL, infoLog));
		LOG_ERROR("LINKING FAILED: {}", infoLog);
		return false;
	}

	return true;
}

bool GLProgram::destroyImplementation()
{
	GL(glDeleteProgram(m_glProgram));

	m_glProgram = 0;

	m_fragmentHandle.unwatch();
	m_fragmentHandle.unload();
	m_vertexHandle.unwatch();
	m_vertexHandle.unload();

	return true;
}

bool GLProgram::isValid()
{
	if (!m_vertexHandle.isValid())
	{
		LOG_ERROR("Invalid vertex handle");
		return false;
	}
	if (!m_fragmentHandle.isValid())
	{
		LOG_ERROR("Invalid fragment handle");
		return false;
	}
	return  true;
}

void GLProgram::use()
{
	GL(glUseProgram(m_glProgram));
}

void GLProgram::unuse()
{
	GL(glUseProgram(0));
}

} // Engine