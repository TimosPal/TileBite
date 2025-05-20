#ifndef GL_TEXTURE_HPP
#define GL_TEXTURE_HPP

#include "core/pch.hpp"
#include "renderer/backend/openGL/GLWrapper.hpp"
#include "resources/Resource.hpp"
#include "resources/ResourceHandle.hpp"
#include "resources/types/ImageResource.hpp"

namespace Engine {

class GLTexture : public Resource<GLTexture> {
	SETUP_ID(Resource, GLTexture)
public:
	GLTexture(const std::string& resourceName, ResourceHandle<ImageResource>&& handle);

	void bind();
	void unbind();

	GLuint getGLID() const { return m_glTexture; }
	bool isValid() override;

	GLTexture(GLTexture&&) noexcept = default;
	GLTexture& operator=(GLTexture&&) noexcept = default;
	GLTexture(const GLTexture&) = delete;
	GLTexture& operator=(const GLTexture&) = delete;
private:
	virtual bool createImplementation() override;
	virtual bool destroyImplementation() override;
	
	GLuint m_glTexture;

	ResourceHandle<ImageResource> m_imageHandle;
};

} // Engine

#endif // !GL_TEXTURE_HPP