#ifndef RESOURCE_REGISTRY_HPP
#define RESOURCE_REGISTRY_HPP

namespace Engine {

namespace ResourceNames {
// Text Files
constexpr const char* SpriteVertFile = "spriteVertFile";
constexpr const char* LineVertFile = "lineVertFile";
constexpr const char* TilemapVertFile = "tilemapVertFile";
constexpr const char* SpriteFragFile = "spriteFragFile";
constexpr const char* LineFragFile = "lineFragFile";

// Images
constexpr const char* MissingImageFile = "missingImageFile";
constexpr const char* WhiteImageFile = "whiteImageFile";

// Vertex shaders
constexpr const char* SpriteVertShader = "spriteVertShader";
constexpr const char* TilemapVertShader = "tilemapVertShader";
constexpr const char* LineVertShader = "lineVertShader";
// Fragment shaders
constexpr const char* SpriteFragShader = "spriteFragShader";
constexpr const char* LineFragShader = "lineFragShader";
// Programs
constexpr const char* SpriteShader = "spriteShader";
constexpr const char* TilemapShader = "tilemapShader";
constexpr const char* LineShader = "lineShader";

// Textures
constexpr const char* FallbackTexture = "fallbackTexture";
constexpr const char* DefaultSpriteTexture = "defaultSpriteTexture";
} // ResourceNames

namespace ResourcePaths {
constexpr const char* ShadersDir = ENGINE_ASSETS_PATH "/shaders/";
constexpr const char* ImagesDir = ENGINE_ASSETS_PATH "/images/";

// Vertex
inline std::string SpriteVertFile() { return ShadersDir + std::string("sprite.vert"); }
inline std::string TilemapVertFile() { return ShadersDir + std::string("tilemap.vert"); }
inline std::string LineVertFile() { return ShadersDir + std::string("line.vert"); }
// Fragment
inline std::string SpriteFragFile() { return ShadersDir + std::string("sprite.frag"); }
inline std::string LineFragFile() { return ShadersDir + std::string("line.frag"); }

// Images
inline std::string MissingImageFile() { return ImagesDir + std::string("missingTexture.png"); }
inline std::string WhiteImageFile() { return ImagesDir + std::string("defaultSprite.png"); }
} // ResourcePaths

} // Engine

#endif // !RESOURCE_REGISTRY_HPP