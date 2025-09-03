#ifndef ENGINE_COMPONENTS_HPP
#define ENGINE_COMPONENTS_HPP

#include <glm/glm.hpp>

#include "utilities/IDGenerator.hpp"
#include "ecs/types/BaseComponent.hpp"
#include "physics/AABB.hpp"
#include "physics/OBB.hpp"
#include "physics/Circle.hpp"


namespace TileBite {

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
	TransformComponent(
		const glm::vec2& position = { 0.0f, 0.0f },
		const glm::vec2& size = { 1.0f, 1.0f },
		float rotation = 0.0f)
		: m_position(position), m_size(size), m_rotation(rotation) {
	}

	const glm::vec2& getPosition() const { return m_position; }
	const glm::vec2& getSize() const { return m_size; }
	const float getRotation() const { return m_rotation; }

	void setPosition(const glm::vec2& position) {
		m_position = position;
		BaseComponent::setDirty(true);
	}

	void setSize(const glm::vec2& size) {
		m_size = size;
		BaseComponent::setDirty(true);
	}

	void setRotation(float rotation) {
		m_rotation = rotation;
		BaseComponent::setDirty(true);
	}

private:
	glm::vec2 m_position;
	glm::vec2 m_size;
	float m_rotation;
};

struct Tile : public BaseComponent {
	glm::vec4 Color;
	uint8_t uIndex;
	uint8_t vIndex;
	bool IsSolid = false;

	Tile(
		const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
		uint8_t u = 0,
		uint8_t v = 0,
		bool isSolid = false)
		: Color(color), uIndex(u), vIndex(v), IsSolid(isSolid) {
	} 
};

struct TilemapComponent : public BaseComponent {
	void setTile(Tile tile, uint8_t xIndex, uint8_t yIndex);

	TilemapComponent(TilemapResource* ptr = nullptr)
		: m_tilemapResource(ptr) {
	}

	TilemapResource* getResource() { return m_tilemapResource; }

private:
	TilemapResource* m_tilemapResource;
};

struct AABBComponent : public BaseComponent {
	AABBComponent() : m_collider(glm::vec2(0.0f), glm::vec2(0.0f)) {}
	AABBComponent(const glm::vec2& min, const glm::vec2& max) : m_collider(min, max) {}

	const AABB& getCollider() { return m_collider; }

	void setSize(const glm::vec2& min, const glm::vec2& max)
	{
		m_collider.setSize(min, max);
		BaseComponent::setDirty(true);
	}

private:
	AABB m_collider;
};

struct OBBComponent : public BaseComponent {
	OBBComponent() : m_collider() {}
	OBBComponent(glm::vec2 center, glm::vec2 size, float rotation) 
		: m_collider(center, size, rotation) {}

	const OBB& getCollider() { return m_collider; }

	void setCenter(const glm::vec2& center) {
		m_collider.Center = center;
		BaseComponent::setDirty(true);
	}

	void setSize(const glm::vec2& size) {
		m_collider.Size = size;
		BaseComponent::setDirty(true);
	}

	void setRotation(float rotation) {
		m_collider.Rotation = rotation;
		BaseComponent::setDirty(true);
	}

private:
	OBB m_collider;
};

struct CircleColliderComponent : public BaseComponent {
	CircleColliderComponent() : m_collider() {}
	CircleColliderComponent(glm::vec2 center, float radius)
		: m_collider(center, radius) {
	}

	const Circle& getCollider() { return m_collider; }

	void setCenter(const glm::vec2& center) {
		m_collider.Center = center;
		BaseComponent::setDirty(true);
	}

	void setSize(float radius) {
		m_collider.Radius = radius;
		BaseComponent::setDirty(true);
	}

private:
	Circle m_collider;
};

struct ParentComponent : public BaseComponent {
	ParentComponent(ID parentID = 0) : m_parentID(parentID) {}

	void setParentID(ID parentID)
	{
		m_parentID = parentID;
		BaseComponent::setDirty(true);
	}

	ID getParentID() const { return m_parentID; }

private:
	ID m_parentID;
};

} // TileBite

#endif // !ENGINE_COMPONENTS_HPP