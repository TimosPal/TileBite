#ifndef RESOURCE_HUB_HPP
#define RESOURCE_HUB_HPP

#include "resources//ResourceManager.hpp"
#include "resources/types/ImageResource.hpp"
#include "resources/types/TextFileResource.hpp"

namespace Engine {

class ResourceHub {
public:
	bool init();

	template<typename T>
	ResourceManager<T>& getManager();

	template<>
	ResourceManager<TextFileResource>& getManager<TextFileResource>() { return m_textFilesResourceManager; }

	template<>
	ResourceManager<ImageResource>& getManager<ImageResource>() { return m_imagesResourceManager; }
private:
	ResourceManager<TextFileResource> m_textFilesResourceManager;
	ResourceManager<ImageResource> m_imagesResourceManager;
};

} // Engine

#endif // !RESOURCE_HUB_HPP
