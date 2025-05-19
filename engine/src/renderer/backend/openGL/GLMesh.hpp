#ifndef GL_MESH_HPP
#define GL_MESH_HPP

#include "core/pch.hpp"
#include "renderer/backend/openGL/GLWrapper.hpp"
#include "renderer/backend/openGL/GLVBO.hpp"
#include "renderer/backend/openGL/GLEBO.hpp"
#include "renderer/VertexLayout.hpp"

namespace Engine {

// OpenGL Mesh is a VAO holding VBOs and EBOs
class GLMesh {
public:
	GLMesh(uint32_t size, uint32_t indicesCount);
	~GLMesh();

	void setVertexData(const void* data, size_t size);
	void setIndexData(const uint32_t* data, size_t count);

	void setupAttributes(const VertexLayout& layout, GLuint shaderProgram);

	void draw(uint32_t indicesCount);

	void bind();
	void unbind();
private:
	GLVBO m_vertexBuffer;
	GLEBO m_indexBuffer;
	GLuint m_glVAO;

	static GLenum getOpenGLBaseType(ShaderAttributeType type);
};

} // Engine

#endif // !GL_MESH_HPP