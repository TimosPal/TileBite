#ifndef ENGINE_COMPONENTS_HPP
#define ENGINE_COMPONENTS_HPP

#include <glm/glm.hpp>

#include "utilities/IDGenerator.hpp"

namespace Engine {

struct SpriteComponent
{
	glm::vec4 Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	
	ID TextureID = 0;

	// Quad uvs (Default set for whole texture)
	glm::vec4 UVRect = glm::vec4(0.0f, 1.0f, 1.0f, 0.0f);
};

struct TransformComponent
{
	glm::vec2 Position = glm::vec2(0, 0);
	glm::vec2 Size = glm::vec2(1, 1);
	float Rotation = 0.0f; // In radians
};

struct Tile {
	glm::vec4 Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	uint8_t uIndex = 0;
	uint8_t vIndex = 0;
};

struct TilemapComponent {
    int Width;
    int Height;
	float WorldTileSize;
	float AtlasTileSize;

	// TODO: use handler ID to avoid pointers, probably safer (?)
    std::vector<uint32_t>* Vertices;
	ID AtlasID = 0;
};

} // Engine

#endif // !ENGINE_COMPONENTS_HPP