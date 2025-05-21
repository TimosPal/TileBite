#include "resources/SystemResourceHub.hpp"
#include "core/ResourceRegistry.hpp"
#include "utilities/misc.hpp"

namespace Engine {

bool SystemResourceHub::init()
{
	bool validAssets = true;

	LOG_INFO("");
	LOG_INFO("====== System resources ======");

	auto spriteVert = m_textFilesResourceManager.addResource(
		TextFileResource(ResourceNames::SpriteVertFile, ResourcePaths::SpriteVertFile())
	);
	validAssets = logResourceValidity(spriteVert, ResourceNames::SpriteVertFile) && validAssets;

	auto spriteFrag = m_textFilesResourceManager.addResource(
		TextFileResource(ResourceNames::SpriteFragFile, ResourcePaths::SpriteFragFile())
	);
	validAssets = logResourceValidity(spriteFrag, ResourceNames::SpriteFragFile) && validAssets;

	auto missingImage = m_imagesResourceManager.addResource(
		ImageResource(ResourceNames::MissingImageFile, ResourcePaths::MissingImageFile())
	);
	validAssets = logResourceValidity(missingImage, ResourceNames::MissingImageFile) && validAssets;

	LOG_INFO("==============================");
	LOG_INFO("");

	return validAssets;
}

bool SystemResourceHub::destroy()
{
	LOG_INFO("");
	LOG_INFO("====== System resources ======");

	m_textFilesResourceManager.clear();
	m_imagesResourceManager.clear();

	LOG_INFO("==============================");
	LOG_INFO("");

	return true;
}

} // Engine