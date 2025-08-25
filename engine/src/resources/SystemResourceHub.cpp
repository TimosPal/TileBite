#include "resources/SystemResourceHub.hpp"
#include "core/ResourceRegistry.hpp"
#include "utilities/misc.hpp"

namespace TileBite {

bool SystemResourceHub::init()
{
	bool validAssets = true;

	#define LOAD_TEXT(resourceName, resourcePath) \
	{ \
		auto resource = m_textFilesResourceManager.addResource(TextFileResource(resourceName, resourcePath)); \
		validAssets = logResourceValidity(resource, resourceName) && validAssets; \
	} \

	#define LOAD_IMAGE(resourceName, resourcePath) \
	{ \
		auto resource = m_imagesResourceManager.addResource(ImageResource(resourceName, resourcePath)); \
		validAssets = logResourceValidity(resource, resourceName) && validAssets; \
	} \

	LOG_INFO("");
	LOG_INFO("====== System resources ======");

	// Vertex
	LOAD_TEXT(ResourceNames::SpriteVertFile, ResourcePaths::SpriteVertFile());
	LOAD_TEXT(ResourceNames::TilemapVertFile, ResourcePaths::TilemapVertFile());
	LOAD_TEXT(ResourceNames::LineVertFile, ResourcePaths::LineVertFile());

	// Fragment
	LOAD_TEXT(ResourceNames::SpriteFragFile, ResourcePaths::SpriteFragFile());
	LOAD_TEXT(ResourceNames::LineFragFile, ResourcePaths::LineFragFile());
	
	// Images
	LOAD_IMAGE(ResourceNames::MissingImageFile, ResourcePaths::MissingImageFile());
	LOAD_IMAGE(ResourceNames::WhiteImageFile, ResourcePaths::WhiteImageFile());

	#undef LOAD_TEXT
	#undef LOAD_IMAGE

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
	m_tilemapResourceManager.clear();

	LOG_INFO("==============================");
	LOG_INFO("");

	return true;
}

} // TileBite