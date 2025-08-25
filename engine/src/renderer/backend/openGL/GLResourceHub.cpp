#include "renderer/backend/openGL/GLResourceHub.hpp"
#include "renderer/backend/openGL/GLShader.hpp"
#include "renderer/backend/openGL/GLProgram.hpp"
#include "core/ResourceRegistry.hpp"
#include "utilities/misc.hpp"

namespace TileBite {

bool GLResourceHub::init()
{
	bool validAssets = true;

	#define LOAD_SHADER(resourceName, textFileName, shaderType) \
	{ \
		auto fileResource = m_systemResourceHub.getManager<TextFileResource>().getResource(textFileName); \
		auto shader = m_shadersResourceManager.addResource( \
			GLShader(resourceName, std::move(fileResource), shaderType) \
		); \
		validAssets = logResourceValidity(shader, resourceName) && validAssets; \
	} \

	#define LOAD_TEXTURE(resourceName, imageFileName) \
	{ \
		auto image = m_systemResourceHub.getManager<ImageResource>().getResource(imageFileName); \
		auto texture = m_texturesResourceManager.addResource( \
			GLTexture(resourceName, std::move(image)) \
		); \
		validAssets = logResourceValidity(texture, resourceName) && validAssets; \
	} \

	#define LOAD_PROGRAM(resourceName, vertShaderName, fragShaderName) \
	{ \
		auto vert = m_shadersResourceManager.getResource(vertShaderName); \
		auto frag = m_shadersResourceManager.getResource(fragShaderName); \
		auto program = m_programsResourceManager.addResource( \
			GLProgram(resourceName, ResourceHandle<GLShader>(vert), ResourceHandle<GLShader>(frag)) \
		); \
		validAssets = logResourceValidity(program, resourceName) && validAssets; \
	} \

	LOG_INFO("");
	LOG_INFO("===== Renderer resources =====");

	// ========= Shaders =========
	LOAD_SHADER(ResourceNames::SpriteVertShader, ResourceNames::SpriteVertFile, ShaderType::Vertex);
	LOAD_SHADER(ResourceNames::TilemapVertShader, ResourceNames::TilemapVertFile, ShaderType::Vertex);
	LOAD_SHADER(ResourceNames::LineVertShader, ResourceNames::LineVertFile, ShaderType::Vertex);

	LOAD_SHADER(ResourceNames::SpriteFragShader, ResourceNames::SpriteFragFile, ShaderType::Fragment);
	LOAD_SHADER(ResourceNames::LineFragShader, ResourceNames::LineFragFile, ShaderType::Fragment);

	// ========= Programs =========
	LOAD_PROGRAM(ResourceNames::SpriteShader, ResourceNames::SpriteVertShader, ResourceNames::SpriteFragShader);
	LOAD_PROGRAM(ResourceNames::TilemapShader, ResourceNames::TilemapVertShader, ResourceNames::SpriteFragShader);
	LOAD_PROGRAM(ResourceNames::LineShader, ResourceNames::LineVertShader, ResourceNames::LineFragShader);

	// ========= Textures =========
	// If this is the first Texture then we can assume default sprite has ID 0 (and set the default spriteComp ID = 0)
	LOAD_TEXTURE(ResourceNames::DefaultSpriteTexture, ResourceNames::WhiteImageFile);
	LOAD_TEXTURE(ResourceNames::FallbackTexture, ResourceNames::MissingImageFile);

	#undef LOAD_SHADER
	#undef LOAD_TEXTURE

	LOG_INFO("==============================");
	LOG_INFO("");

	return validAssets;
}

bool GLResourceHub::destroy()
{
	// NOTE: order matters!!!
	// Managers using resources from other managers should be destroyed first.

	LOG_INFO("");
	LOG_INFO("===== Renderer resources =====");

	m_programsResourceManager.clear();
	m_shadersResourceManager.clear();
	m_texturesResourceManager.clear();

	LOG_INFO("==============================");

	return true;
}

} // TileBite