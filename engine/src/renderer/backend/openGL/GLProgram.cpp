#include "renderer/backend/openGL/GLProgram.hpp"

#include "renderer/backend/openGL/GLWrapper.hpp"
#include "utilities/Logger.hpp"

namespace Engine {

GLProgram::GLProgram()
{}

GLProgram::GLProgram(
	const std::string& resourceName,
	ResourceHandle<GLShader>&& vertexHandle,
	ResourceHandle<GLShader>&& fragmentHandle
) 
	: m_vertexHandle(vertexHandle), m_fragmentHandle(fragmentHandle)
{}

bool GLProgram::createImplementation()
{
	m_vertexHandle.load();
	m_fragmentHandle.load();

	GL_RET(glCreateProgram(), m_glProgram);

	unsigned int vertexShader = m_vertexHandle.getResource()->getGLID();
	unsigned int fragmentShader = m_fragmentHandle.getResource()->getGLID();

	GL(glAttachShader(m_glProgram, vertexShader));
	GL(glAttachShader(m_glProgram, fragmentShader));
	GL(glLinkProgram(m_glProgram));

	int success;
	char infoLog[512];
	GL(glGetProgramiv(m_glProgram, GL_LINK_STATUS, &success));
	if (!success)
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

	if (s_instanceInUse == this)
	{
		s_instanceInUse = nullptr;
	}
	m_glProgram = 0;

	return true;
}

bool GLProgram::isValid()
{
	return m_vertexHandle.isValid() && m_fragmentHandle.isValid();
}

void GLProgram::use()
{
	if (s_instanceInUse == this)
	{
		return;
	}

	s_instanceInUse = this;
	GL(glUseProgram(m_glProgram));
}

void GLProgram::unuse()
{
	if (s_instanceInUse != this)
	{
		return;
	}

	s_instanceInUse = nullptr;
	GL(glUseProgram(0));
}

} // Engine