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
	SpriteComponent sprite;
	TransformComponent transform;
};

struct TilemapComponent {
    int width;
    int height;
	float tileSize;
    Tile tiles[4000]; // Flattened 1D array: tiles[y * width + x]

    // Helper to get tile at (x, y)
    inline Tile& getTile(int x, int y) {
        return tiles[y * width + x];
    }
};

} // Engine

#endif // !ENGINE_COMPONENTS_HPP