#ifndef IMAGE_RESOURCE_HPP
#define IMAGE_RESOURCE_HPP

#include "core/pch.hpp"
#include "resources/Resource.hpp"

namespace Engine {

class ImageResource : public Resource<ImageResource> {
	SETUP_ID(Resource, ImageResource)
public:
	ImageResource(const std::string& resourceName, const std::string& filePath);
	virtual bool isValid() override;

	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }
	int getChannels() const { return m_channels; }
	unsigned char* getData() const { return m_data; }

	ImageResource(ImageResource&&) noexcept = default;
	ImageResource& operator=(ImageResource&&) noexcept = default;
	ImageResource(const ImageResource&) = delete;
	ImageResource& operator=(const ImageResource&) = delete;
private:
	virtual bool createImplementation() override;
	virtual bool destroyImplementation() override;

	std::string m_filePath;
	unsigned char* m_data;
	int m_width, m_height, m_channels;
};

} // Engine

#endif // !IMAGE_RESOURCE_HPP
