#include "renderer/backend/openGL/GLResourceHub.hpp"
#include "renderer/backend/openGL/GLShader.hpp"
#include "renderer/backend/openGL/GLProgram.hpp"
#include "core/ResourceRegistry.hpp"
#include "utilities/misc.hpp"

namespace Engine {

bool GLResourceHub::init()
{
	bool validAssets = true;

	LOG_INFO("");
	LOG_INFO("===== Renderer resources =====");

	auto spriteVertFile = m_systemResourceHub.getManager<TextFileResource>().getResource(ResourceNames::SpriteVertFile);
	auto spriteVertShader = m_shadersResourceManager.addResource(
		GLShader(ResourceNames::SpriteVertShader, std::move(spriteVertFile), ShaderType::Vertex)
	);
	validAssets = logResourceValidity(spriteVertShader, ResourceNames::SpriteVertShader) && validAssets;

	auto spriteFragFile = m_systemResourceHub.getManager<TextFileResource>().getResource(ResourceNames::SpriteFragFile);
	auto spriteFragShader = m_shadersResourceManager.addResource(
		GLShader(ResourceNames::SpriteFragShader, std::move(spriteFragFile), ShaderType::Fragment)
	);
	validAssets = logResourceValidity(spriteFragShader, ResourceNames::SpriteFragShader) && validAssets;

	auto spriteShader = m_programsResourceManager.addResource(
		GLProgram(ResourceNames::SpriteShader, std::move(spriteVertShader), std::move(spriteFragShader))
	);
	validAssets = logResourceValidity(spriteShader, ResourceNames::SpriteShader) && validAssets;

	// If this is the first Texture then we can assume default sprite has ID 0 (and set the default spriteComp ID = 0)
	auto whiteImage = m_systemResourceHub.getManager<ImageResource>().getResource(ResourceNames::WhiteImageFile);
	auto defaultSpriteTexture = m_texturesResourceManager.addResource(
		GLTexture(ResourceNames::DefaultSpriteTexture, std::move(whiteImage))
	);
	validAssets = logResourceValidity(defaultSpriteTexture, ResourceNames::DefaultSpriteTexture) && validAssets;

	auto fallbackImage = m_systemResourceHub.getManager<ImageResource>().getResource(ResourceNames::MissingImageFile);
	auto fallbackTexture = m_texturesResourceManager.addResource(
		GLTexture(ResourceNames::FallbackTexture, std::move(fallbackImage))
	);
	validAssets = logResourceValidity(fallbackTexture, ResourceNames::FallbackTexture) && validAssets;

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

} // Engine