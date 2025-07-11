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

	case ShaderAttributeType::UInt:
	case ShaderAttributeType::UInt2:
	case ShaderAttributeType::UInt3:
	case ShaderAttributeType::UInt4:
		return GL_UNSIGNED_INT;

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

void GLMesh::setSubVertexData(const void* data, size_t size, size_t offset)
{
	m_vertexBuffer.setSubData(data, size, offset);
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
		GL_RET(glGetAttribLocation(shaderProgram, attribute.Name.c_str()), location);
		ASSERT(location >= 0, "Invalid location of shader attribute");
		GL(glEnableVertexAttribArray(location));

		bool isInteger =
			attribute.Type == ShaderAttributeType::Int || attribute.Type == ShaderAttributeType::Int2 ||
			attribute.Type == ShaderAttributeType::Int3 || attribute.Type == ShaderAttributeType::Int4 ||
			attribute.Type == ShaderAttributeType::UInt || attribute.Type == ShaderAttributeType::UInt2 ||
			attribute.Type == ShaderAttributeType::UInt3 || attribute.Type == ShaderAttributeType::UInt4;

		if (isInteger) {
			GL(glVertexAttribIPointer(
				location,
				attribute.ElementsCount,
				getOpenGLBaseType(attribute.Type),
				layout.getStride(),
				(const void*)(uintptr_t)attribute.Offset
			));
		}
		else {
			GL(glVertexAttribPointer(
				location,
				attribute.ElementsCount,
				getOpenGLBaseType(attribute.Type),
				GL_FALSE,
				layout.getStride(),
				(const void*)(uintptr_t)attribute.Offset
			));
		}
	}
}

void GLMesh::drawIndexed(uint32_t indicesCount)
{
	// Draw using index buffer (EBO)
	GL(glBindVertexArray(m_glVAO));
	GL(glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr));
}

void GLMesh::drawLines(uint32_t verticesCount)
{
	GL(glLineWidth(2.0f)); // TODO: Make line width configurable 
	GL(glBindVertexArray(m_glVAO));
	GL(glDrawArrays(GL_LINES, 0, verticesCount));
}

} // Engine