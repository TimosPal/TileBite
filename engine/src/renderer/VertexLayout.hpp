#ifndef VERTEX_LAYOUT_HPP
#define VERTEX_LAYOUT_HPP

#include "core/pch.hpp"
#include "renderer/VertexAttribute.hpp"

namespace Engine {

class VertexLayout {
public:
	void add(VertexAttribute&& attribute)
	{
		attribute.Offset = m_stride;
		m_stride += attribute.Size;
		m_attributes.emplace_back(std::move(attribute));
	}

	const std::vector<VertexAttribute>& getLayout() const { return m_attributes; }
	uint32_t getStride() const { return m_stride; }
private:
	std::vector<VertexAttribute> m_attributes;
	uint32_t m_stride = 0;
};

} // Engine

#endif // !VERTEX_LAYOUT_HPP