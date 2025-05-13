#include "resources/types/ImageResource.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Engine {

ImageResource::ImageResource()
	: Resource<ImageResource>()
{}

ImageResource::ImageResource(const std::string& resourceName, const std::string& filePath)
	: Resource<ImageResource>(resourceName)
	, m_filePath(filePath)
{}

bool ImageResource::createImplementation()
{
	stbi_set_flip_vertically_on_load(true);
	m_data = stbi_load(m_filePath.c_str(), &m_width, &m_height, &m_channels, 0);
	if (!m_data)
	{
		return false;
	}

	return true;
}

bool ImageResource::destroyImplementation()
{
	if (m_data)
	{
		stbi_image_free(m_data);
		m_data = nullptr;
		m_width = m_height = m_channels = 0;
	}

	return true;
}

bool ImageResource::isValid()
{
	bool exists = std::filesystem::exists(m_filePath);
	return exists;
}

} // Engine