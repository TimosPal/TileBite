#ifndef GL_EBO_HPP
#define GL_EBO_HPP

#include "core/Types.hpp"
#include "renderer/backend/openGL/GLWrapper.hpp"

namespace TileBite {

// OpenGL Element Buffer Object (Indices)
class GLEBO {
public:
	GLEBO(const uint32_t* data, uint32_t count);
	GLEBO(uint32_t count) : GLEBO(nullptr, count) {}
	~GLEBO();

	void bind();
	void unbind();

	void setData(const uint32_t* data, uint32_t count);

	uint32_t getCount() const { return m_count; }
private:
	GLuint m_glEBO;
	uint32_t m_count;
};

} // TileBite

#endif // !GL_EBO_HPP