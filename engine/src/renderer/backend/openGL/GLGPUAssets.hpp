#ifndef GPU_ASSETS_HPP
#define GPU_ASSETS_HPP

#include "renderer/IGPUAssets.hpp"

namespace Engine {

class GLGPUAssets : public IGPUAssets {
public:
	GLGPUAssets(GLResourceHub& resourceHub) : m_resourceHub(resourceHub) {}

	ID createTexture(std::string resourceName, ResourceHandle<ImageResource>&& imageHandle) override
	{
		auto glTextureHandle = m_resourceHub.getManager<GLTexture>().addResource(
			GLTexture(resourceName, std::move(imageHandle))
		);
		return glTextureHandle.getResource()->getInstanceID();
	}

private:
	GLResourceHub& m_resourceHub;
};

} // Engine

#endif // !GPU_ASSETS_HPP