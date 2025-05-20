#include "renderer/backend/openGL/GLMesh.hpp"

namespace Engine {

GLenum GLMesh::getOpenGLBaseType(ShaderAttributeType type) {
	switch (type) {
	case ShaderAttributeType::Float:
	case ShaderAttributeType::Float2:
	case ShaderAttributeType::Float3:
	case ShaderAttributeType::Float4:
	case ShaderAttributeType::Mat3:
	case ShaderAttributeType::Mat4:
		return GL_FLOAT;

	case ShaderAttributeType::Int:
	case ShaderAttributeType::Int2:
	case ShaderAttributeType::Int3:
	case ShaderAttributeType::Int4:
		return GL_INT;

	case ShaderAttributeType::Bool:
		return GL_BOOL;

	default:
		ASSERT_FALSE("Unsupported shader attribute type");
		return 0;
	}
}

GLMesh::GLMesh(uint32_t size, uint32_t indicesCount)
	: m_indexBuffer(indicesCount), m_vertexBuffer(size)
{
	GL(glCreateVertexArrays(1, &m_glVAO));
}

GLMesh::~GLMesh()
{
	GL(glDeleteVertexArrays(1, &m_glVAO));
}

void GLMesh::setVertexData(const void* data, size_t size)
{
	m_vertexBuffer.setData(data, size);
}

void GLMesh::setIndexData(const uint32_t* data, size_t count)
{
	m_indexBuffer.setData(data, count);
}

void GLMesh::bind()
{
	GL(glBindVertexArray(m_glVAO));
}

void GLMesh::unbind()
{
	GL(glBindVertexArray(0));
}

void GLMesh::setupAttributes(const VertexLayout& layout, GLuint shaderProgram)
{
	// Bind (VAO)
	GL(glBindVertexArray(m_glVAO)); 

	m_vertexBuffer.bind();
	m_indexBuffer.bind();

	for (const auto& attribute : layout.getLayout())
	{
		GLint location;
		LOG_INFO("{} {}", attribute.Name, shaderProgram);
		GL_RET(glGetAttribLocation(shaderProgram, attribute.Name.c_str()), location);
		ASSERT(location >= 0, "Invalid location of shader attribute");
		GL(glEnableVertexAttribArray(location));
		GL(glVertexAttribPointer(
			location, // Location
			attribute.ElementsCount, // Element count (eg vec2 = 2)
			getOpenGLBaseType(attribute.Type), // GLSL attribute type converted to glad GL type 
			GL_FALSE, // Normalised values (default to false)
			layout.getStride(), // Size of whole vertex stride
			(const void*)(uintptr_t)attribute.Offset // Offset between elements
		));
	}
}

void GLMesh::draw(uint32_t indicesCount)
{
	GL(glBindVertexArray(m_glVAO));
	GL(glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr));
}

} // Engine