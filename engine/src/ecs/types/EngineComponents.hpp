#ifndef ENGINE_COMPONENTS_HPP
#define ENGINE_COMPONENTS_HPP

#include <glm/glm.hpp>

#include "utilities/IDGenerator.hpp"
#include "ecs/types/BaseComponent.hpp"

namespace Engine {

class TilemapResource;

struct SpriteComponent : public BaseComponent
{
	glm::vec4 Color;
	ID TextureID;
	glm::vec4 UVRect;

	SpriteComponent(
		const glm::vec4& color = glm::vec4(1.0f),
		ID textureID = 0,
		const glm::vec4& uvRect = glm::vec4(0.0f, 1.0f, 1.0f, 0.0f))
		: Color(color), TextureID(textureID), UVRect(uvRect) {
	}
};

struct TransformComponent : public BaseComponent {
	glm::vec2 Position;
	glm::vec2 Size;
	float Rotation;

	TransformComponent(
		const glm::vec2& position = { 0.0f, 0.0f },
		const glm::vec2& size = { 1.0f, 1.0f },
		float rotation = 0.0f)
		: Position(position), Size(size), Rotation(rotation) {
	}

	void setPosition(const glm::vec2& position) {
		Position = position;
		BaseComponent::setDirty(true);
	}

	void setSize(const glm::vec2& size) {
		Size = size;
		BaseComponent::setDirty(true);
	}

	void setRotation(float rotation) {
		Rotation = rotation;
		BaseComponent::setDirty(true);
	}
};

struct Tile : public BaseComponent {
	glm::vec4 Color;
	uint8_t uIndex;
	uint8_t vIndex;

	Tile(
		const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
		uint8_t u = 0,
		uint8_t v = 0)
		: Color(color), uIndex(u), vIndex(v) {
	}
};

struct TilemapComponent : public BaseComponent {
	TilemapResource* TilemapResourcePtr;

	TilemapComponent(TilemapResource* ptr = nullptr)
		: TilemapResourcePtr(ptr) {
	}
};

} // Engine

#endif // !ENGINE_COMPONENTS_HPP