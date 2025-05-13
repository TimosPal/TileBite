#ifndef RESOURCE_REGISTRY_HPP
#define RESOURCE_REGISTRY_HPP

namespace Engine {

namespace ResourceNames {
constexpr const char* SpriteVertFile = "spriteVertFile";
constexpr const char* SpriteFragFile = "spriteFragFile";
constexpr const char* MissingImageFile = "missingImageFile";

constexpr const char* SpriteVertShader = "spriteVertShader";
constexpr const char* SpriteFragShader = "spriteFragShader";
constexpr const char* SpriteShader = "spriteShader";
} // ResourceNames

namespace ResourcePaths {
constexpr const char* ShadersDir = ENGINE_ASSETS_PATH "/shaders/";
constexpr const char* ImagesDir = ENGINE_ASSETS_PATH "/images/";

inline std::string SpriteFragFile() { return std::string(ShadersDir) + "sprite.frag"; }
inline std::string SpriteVertFile() { return std::string(ShadersDir) + "sprite.vert"; }
inline std::string MissingImageFile() { return std::string(ImagesDir) + "missingTexture.png"; }
} // ResourcePaths

} // Engine

#endif // !RESOURCE_REGISTRY_HPP