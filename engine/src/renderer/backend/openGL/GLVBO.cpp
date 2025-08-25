#include "renderer/backend/openGL/GLVBO.hpp"

#include "utilities/assertions.hpp"

namespace TileBite {

GLVBO::GLVBO(const float* data, uint32_t size)
	: m_size(size)
{
	GL(glCreateBuffers(1, &m_glVBO));
	GL(glBindBuffer(GL_ARRAY_BUFFER, m_glVBO));
	GL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

GLVBO::~GLVBO()
{
	GL(glDeleteBuffers(1, &m_glVBO));
}

void GLVBO::bind()
{
	GL(glBindBuffer(GL_ARRAY_BUFFER, m_glVBO));
}

void GLVBO::unbind()
{
	GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void GLVBO::setData(const void* data, uint32_t size)
{
	GL(glBindBuffer(GL_ARRAY_BUFFER, m_glVBO));
	GL(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
}

void GLVBO::setSubData(const void* data, uint32_t size, uint32_t offset) {
	GL(glBindBuffer(GL_ARRAY_BUFFER, m_glVBO));
	GL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
}

}