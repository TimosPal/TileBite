#include "renderer/backend/openGL/GLEBO.hpp"

#include "utilities/assertions.hpp"

namespace Engine {

GLEBO::GLEBO(const uint32_t* data, uint32_t count)
	: m_count(count)
{
	if (count == 0) return; // Not using indices, do not make buffers in GPU

	GL(glCreateBuffers(1, &m_glEBO));
	GL(glBindBuffer(GL_ARRAY_BUFFER, m_glEBO));
	GL(glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW));
}

GLEBO::~GLEBO()
{
	GL(glDeleteBuffers(1, &m_glEBO));
}

void GLEBO::bind()
{
	GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glEBO));
}

void GLEBO::unbind()
{
	GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void GLEBO::setData(const uint32_t* data, uint32_t count)
{
	GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glEBO));
	GL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, count * sizeof(uint32_t), data));
}

} // Engine