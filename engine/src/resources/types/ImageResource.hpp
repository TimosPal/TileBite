#ifndef IMAGE_RESOURCE_HPP
#define IMAGE_RESOURCE_HPP

#include "core/pch.hpp"
#include "resources/Resource.hpp"

namespace Engine {

class ImageResource : public Resource<ImageResource> {
	SETUP_ID(Resource, ImageResource)
public:
	ImageResource();
	ImageResource(const std::string& resourceName, const std::string& filePath);
private:
	virtual bool createImplementation() override;
	virtual bool destroyImplementation() override;

	std::string m_filePath;
	unsigned char* m_data;
	int m_width, m_height, m_channels;
};

} // Engine

#endif // !IMAGE_RESOURCE_HPP
