#include "GLTexture.hpp"

namespace Engine {

GLTexture::GLTexture(const std::string& resourceName, ResourceHandle<ImageResource>&& handle)
	: Resource<GLTexture>(resourceName, false),
	m_imageHandle(std::move(handle)), m_glTexture(0)
{
}

void GLTexture::bind()
{
	GL(glBindTexture(GL_TEXTURE_2D, m_glTexture));
}

void GLTexture::unbind()
{
	GL(glBindTexture(GL_TEXTURE_2D, 0));
}

bool GLTexture::isValid()
{
	if (!m_imageHandle.isValid())
	{
		LOG_ERROR("Invalid image handle");
		return false;
	}
	return true;
}

bool GLTexture::createImplementation()
{
	// TODO: 
	// 1) refactor to use constructor arguments for wrapping / filtering
	// 2) Mipmap generation (check if done properly)

	GL(glGenTextures(1, &m_glTexture));
	GL(glBindTexture(GL_TEXTURE_2D, m_glTexture));

	// set the texture wrapping / filtering options
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	m_imageHandle.watch();
	bool loadedImage = m_imageHandle.load();
	ASSERT(loadedImage, "Failed to load texture image");
	auto imageResource = m_imageHandle.getResource();

	GL(glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		imageResource->getWidth(),
		imageResource->getHeight(),
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		imageResource->getData()
	));

	GL(glGenerateMipmap(GL_TEXTURE_2D));

	return true;
}

bool GLTexture::destroyImplementation()
{
	GL(glDeleteTextures(1, &m_glTexture));
	m_imageHandle.unwatch();
	return true;
}

} // Engine