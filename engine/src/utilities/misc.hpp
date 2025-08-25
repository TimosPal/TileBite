#ifndef MISC_HPP
#define MISC_HPP

#include "core/pch.hpp"
#include "utilities/Logger.hpp"
#include "ecs/types/EngineComponents.hpp"

namespace TileBite {

constexpr int PACKED_TILEMAP_QUAD_BYTES = 8;

template<typename Derived, typename Base>
concept DerivedFrom = std::is_base_of_v<Base, Derived>;

template<typename T>
concept DefaultConstructible = std::is_default_constructible_v<T>;

template<typename T>
bool removeElement(std::vector<T>& vec, int index)
{
    // Swap remove idiom.
    if (index >= 0 && index < vec.size())
    {
        std::swap(vec[index], vec.back()); 
        vec.pop_back();
        return true;
    }
    return false;
}

template<typename ResourceTypeHandle>
bool logResourceValidity(ResourceTypeHandle& handle, const char* name)
{
	if (handle.isValid())
	{
		LOG_INFO("Valid resource: {}", name);
		return true;
	}
	else
	{
		LOG_ERROR("Invalid resource: {}", name);
		return false;
	}
}

inline std::array<float, 12> makeLineVerticesColored(glm::vec2 start, glm::vec2 end, glm::vec4 color)
{
	return {
		start.x, start.y, color.r, color.g, color.b, color.a,
		end.x,   end.y,   color.r, color.g, color.b, color.a
	};
}

// TODO: move elsewhere (More appropriate file)
inline std::array<float, 36> makeSpriteQuadVertices(TransformComponent* t, SpriteComponent* spr)
{
	auto& pos = t->getPosition();
	auto& size = t->getSize();
	float angle = t->getRotation();

	// Small explanaiton of the following math:
	// We apply transformations in the following order
	// (Matrices are too slow for cpu calculations!)
	// 1) scale (0.5 due to the local quad size)
	// 2) vertex rotated by angle (in radians)
	// Using formulas:
	// [x' = x cos(angle) - y sin(angle)]
	// [y' = x sin(angle) + y cos(angle)]
	// 3) vertex moved to position

	float cosA = cos(angle);
	float sinA = sin(angle);
	float hw = size.x * 0.5f;
	float hh = size.y * 0.5f;

	// Quad corners after scale + rotation + translation
	float topLeftX = pos.x + (-hw * cosA - hh * sinA);
	float topLeftY = pos.y + (-hw * sinA + hh * cosA);

	float topRightX = pos.x + (hw * cosA - hh * sinA);
	float topRightY = pos.y + (hw * sinA + hh * cosA);

	float bottomRightX = pos.x + (hw * cosA - (-hh) * sinA);
	float bottomRightY = pos.y + (hw * sinA + (-hh) * cosA);

	float bottomLeftX = pos.x + (-hw * cosA - (-hh) * sinA);
	float bottomLeftY = pos.y + (-hw * sinA + (-hh) * cosA);

	float r = spr->Color.r;
	float g = spr->Color.g;
	float b = spr->Color.b;
	float a = spr->Color.a;

	float u0 = spr->UVRect.x;
	float v0 = spr->UVRect.y;
	float u1 = spr->UVRect.z;
	float v1 = spr->UVRect.w;

	ID textureID = spr->TextureID;

	return std::array<float, 36>{
		// pos						  // color      // uv     // texture
		topLeftX, topLeftY,           r, g, b, a,   u0, v0,   float(textureID),
		topRightX, topRightY,         r, g, b, a,   u1, v0,   float(textureID),
		bottomRightX, bottomRightY,   r, g, b, a,   u1, v1,   float(textureID),
		bottomLeftX, bottomLeftY,     r, g, b, a,   u0, v1,   float(textureID)
	};
}

inline uint32_t packXYIndexUV(uint8_t x, uint8_t y, uint8_t uvx, uint8_t uvy) {
	return (uint32_t(x)) |
		(uint32_t(y) << 8) |
		(uint32_t(uvx) << 16) |
		(uint32_t(uvy) << 24);
}

inline uint32_t packRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	return (uint32_t(r)) |
		(uint32_t(g) << 8) |
		(uint32_t(b) << 16) |
		(uint32_t(a) << 24);
}

inline void unpackXYIndexUV(uint32_t packed,
	uint8_t& x, uint8_t& y,
	uint8_t& uvx, uint8_t& uvy)
{
	x = static_cast<uint8_t>(packed & 0xFF);
	y = static_cast<uint8_t>((packed >> 8) & 0xFF);
	uvx = static_cast<uint8_t>((packed >> 16) & 0xFF);
	uvy = static_cast<uint8_t>((packed >> 24) & 0xFF);
}

inline void unpackRGBA(uint32_t packed,
	uint8_t& r, uint8_t& g,
	uint8_t& b, uint8_t& a)
{
	r = static_cast<uint8_t>(packed & 0xFF);
	g = static_cast<uint8_t>((packed >> 8) & 0xFF);
	b = static_cast<uint8_t>((packed >> 16) & 0xFF);
	a = static_cast<uint8_t>((packed >> 24) & 0xFF);
}

inline std::array<uint32_t, PACKED_TILEMAP_QUAD_BYTES> makePackedTilemapQuad(
	uint8_t xIndex, uint8_t yIndex,
	uint8_t uIndex, uint8_t vIndex,
	glm::vec4 color)
{
	glm::u8vec4 u8Color = glm::u8vec4(color * 255.0f); // Ensure color is in [0, 255] range
	uint32_t packedColor = packRGBA(u8Color.r, u8Color.g, u8Color.b, u8Color.a);

	return std::array<uint32_t, PACKED_TILEMAP_QUAD_BYTES>{
			packXYIndexUV(xIndex, yIndex, uIndex, vIndex),     // top-left
			packedColor,
			packXYIndexUV(xIndex + 1, yIndex, uIndex + 1, vIndex),     // top-right
			packedColor,
			packXYIndexUV(xIndex + 1, yIndex + 1, uIndex + 1, vIndex + 1), // bottom-right
			packedColor,
			packXYIndexUV(xIndex, yIndex + 1, uIndex, vIndex + 1), // bottom-left
			packedColor
	};
}

inline void unpackTileData(uint32_t* packedData,
	uint8_t& xIndex, uint8_t& yIndex,
	uint8_t& uIndex, uint8_t& vIndex,
	glm::vec4& color)
{
	uint8_t r, g, b, a;

	// Unpack indices
	unpackXYIndexUV(packedData[0], xIndex, yIndex, uIndex, vIndex);

	// Unpack color
	unpackRGBA(packedData[1], r, g, b, a);

	// Normalize color to 0-1 range
	constexpr float inv255 = 1.0f / 255.0f;
	color = glm::vec4(r * inv255, g * inv255, b * inv255, a * inv255);
}

inline std::vector<uint32_t> makeIndices(int indicesCount, int verticesPerQuad, int indicesPerQuad, int quadCount)
{
	std::vector<uint32_t> indexData(indicesCount);
	for (size_t i = 0; i < quadCount; ++i)
	{
		uint32_t offset = i * verticesPerQuad;
		size_t idx = i * indicesPerQuad;

		indexData[idx + 0] = offset + 0;
		indexData[idx + 1] = offset + 1;
		indexData[idx + 2] = offset + 2;
		indexData[idx + 3] = offset + 2;
		indexData[idx + 4] = offset + 3;
		indexData[idx + 5] = offset + 0;
	}

	return indexData;
}

inline float quickRandFloat(float min = -1.0f, float max = 1.0f) {
	return min + (max - min) * (rand() / float(RAND_MAX));
}

} // TileBite

#endif // !MISC_HPP
