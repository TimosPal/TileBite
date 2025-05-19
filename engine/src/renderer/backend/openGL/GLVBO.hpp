#ifndef GL_VBO_HPP
#define GL_VBO_HPP

#include "core/Types.hpp"
#include "renderer/backend/openGL/GLWrapper.hpp"

namespace Engine {

// OpenGL Vertex Buffer Object (vertices)
class GLVBO {
public:
	GLVBO(const float* data, uint32_t size);
	GLVBO(uint32_t size) : GLVBO(nullptr, size) {}
	~GLVBO();

	void bind();
	void unbind();

	void setData(const void* data, uint32_t size);
private:
	GLuint m_glVBO;
	uint32_t m_size;
};

} // Engine

#endif // !GL_VBO_HPP