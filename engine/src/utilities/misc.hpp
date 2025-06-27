#ifndef MISC_HPP
#define MISC_HPP

#include "core/pch.hpp"
#include "utilities/Logger.hpp"
#include "ecs/types/EngineComponents.hpp"

namespace Engine {

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

// TODO: move elsewhere (More appropriate file)
inline std::array<float, 36> makeSpriteQuadVertices(TransformComponent* t, SpriteComponent* spr)
{
	auto& pos = t->Position;
	auto& size = t->Size;
	float angle = t->Rotation;

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

inline std::array<uint32_t, 8> makePackedTilemapQuad(
	uint8_t xIndex, uint8_t yIndex,
	uint8_t uIndex, uint8_t vIndex,
	const glm::u8vec4& color)
{
	uint32_t packedColor = packRGBA(color.r, color.g, color.b, color.a);

	return std::array<uint32_t, 8>{
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

inline float quickRandFloat(float min = -1.0f, float max = 1.0f) {
	return min + (max - min) * (rand() / float(RAND_MAX));
}

} // Engine

#endif // !MISC_HPP
