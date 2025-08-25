#ifndef SYSTEM_RESOURCE_HUB_HPP
#define SYSTEM_RESOURCE_HUB_HPP

#include "resources/ResourceManager.hpp"
#include "resources/types/ImageResource.hpp"
#include "resources/types/TextFileResource.hpp"
#include "resources/types/TilemapResource.hpp"

namespace TileBite {

class SystemResourceHub {
public:
	bool init();
	bool destroy();

	template<typename T>
	ResourceManager<T>& getManager();

	template<>
	ResourceManager<TextFileResource>& getManager<TextFileResource>() { return m_textFilesResourceManager; }

	template<>
	ResourceManager<ImageResource>& getManager<ImageResource>() { return m_imagesResourceManager; }

	template<>
	ResourceManager<TilemapResource>& getManager<TilemapResource>() { return m_tilemapResourceManager; }
private:
	ResourceManager<TextFileResource> m_textFilesResourceManager;
	ResourceManager<ImageResource> m_imagesResourceManager;
	ResourceManager<TilemapResource> m_tilemapResourceManager;
};

} // TileBite

#endif // !SYSTEM_RESOURCE_HUB_HPP