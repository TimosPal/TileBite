#ifndef VERTEX_ATTRIBUTE_HPP
#define VERTEX_ATTRIBUTE_HPP

#include "core/pch.hpp"
#include "utilities/assertions.hpp"

namespace TileBite {

enum class ShaderAttributeType {
    None = 0,
    Float, Float2, Float3, Float4,
    Mat3, Mat4,
    Int, Int2, Int3, Int4,
    UInt, UInt2, UInt3, UInt4,
    Bool
};

struct VertexAttribute {
	std::string Name;
	ShaderAttributeType Type;
	uint32_t Size = 0;
	uint32_t Offset = 0;
	uint32_t ElementsCount = 0;

    static uint32_t getShaderTypeSize(ShaderAttributeType type);
    static uint32_t getComponentCount(ShaderAttributeType type);

	VertexAttribute(const std::string& name, ShaderAttributeType type)
		: 
        Name(name),
        Type(type),
        ElementsCount(getComponentCount(type)),
        Size(getShaderTypeSize(type)),
        Offset(0)
    {}
};

inline uint32_t VertexAttribute::getShaderTypeSize(ShaderAttributeType type) {
    switch (type) {
    case ShaderAttributeType::Float:  return sizeof(float);
    case ShaderAttributeType::Float2: return sizeof(float) * 2;
    case ShaderAttributeType::Float3: return sizeof(float) * 3;
    case ShaderAttributeType::Float4: return sizeof(float) * 4;
    case ShaderAttributeType::Mat3:   return sizeof(float) * 3 * 3;
    case ShaderAttributeType::Mat4:   return sizeof(float) * 4 * 4;
    case ShaderAttributeType::Int:    return sizeof(int);
    case ShaderAttributeType::Int2:   return sizeof(int) * 2;
    case ShaderAttributeType::Int3:   return sizeof(int) * 3;
    case ShaderAttributeType::Int4:   return sizeof(int) * 4;
	case ShaderAttributeType::UInt:   return sizeof(uint32_t);
	case ShaderAttributeType::UInt2:  return sizeof(uint32_t) * 2;
	case ShaderAttributeType::UInt3:  return sizeof(uint32_t) * 3;
	case ShaderAttributeType::UInt4:  return sizeof(uint32_t) * 4;
    case ShaderAttributeType::Bool:   return sizeof(bool);
    default:
        ASSERT_FALSE("Unsupported shader attribute type");
        return 0;
    }
}

inline uint32_t VertexAttribute::getComponentCount(ShaderAttributeType type) {
    switch (type) {
    case ShaderAttributeType::Float:  return 1;
    case ShaderAttributeType::Float2: return 2;
    case ShaderAttributeType::Float3: return 3;
    case ShaderAttributeType::Float4: return 4;
    case ShaderAttributeType::Mat3:   return 3 * 3;
    case ShaderAttributeType::Mat4:   return 4 * 4;
    case ShaderAttributeType::Int:    return 1;
    case ShaderAttributeType::Int2:   return 2;
    case ShaderAttributeType::Int3:   return 3;
    case ShaderAttributeType::Int4:   return 4;
	case ShaderAttributeType::UInt:   return 1;
	case ShaderAttributeType::UInt2:  return 2;
	case ShaderAttributeType::UInt3:  return 3;
	case ShaderAttributeType::UInt4:  return 4;
    case ShaderAttributeType::Bool:   return 1;
    default:
        ASSERT_FALSE("Unsupported shader attribute type");
        return 0;
    }
}

} // TileBite

#endif // !VERTEX_ATTRIBUTE_HPP
