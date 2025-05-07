#include "resources/types/ResourceHub.hpp"

namespace Engine {

bool ResourceHub::init()
{
	bool loadedAssets = true;

	std::string root = ENGINE_ASSETS_PATH;
	LOG_INFO("{}", root);

	auto spriteFrag = m_textFilesResourceManager.addResource(TextFileResource("spriteFrag", root + "/shaders/sprite.frag"));
	loadedAssets = spriteFrag.load() && loadedAssets;

	auto spriteVert = m_textFilesResourceManager.addResource(TextFileResource("spriteVert", root + "/shaders/sprite.vert"));
	loadedAssets = spriteVert.load() && loadedAssets;

	auto missingImage = m_imagesResourceManager.addResource(ImageResource("missingImage", root + "/images/missingTexture.png"));
	loadedAssets = missingImage.load() && loadedAssets;

	return loadedAssets;
}

} // Engine