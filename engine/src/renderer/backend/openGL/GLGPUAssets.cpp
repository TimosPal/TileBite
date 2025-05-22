#include "renderer/backend/openGL/GLGPUAssets.hpp"

namespace Engine {

GLGPUAssets::GLGPUAssets(GLResourceHub& resourceHub) : m_resourceHub(resourceHub) {}

ID GLGPUAssets::getTexture(std::string resourceName)
{
	return m_resourceHub.getManager<GLTexture>().getResourceID(resourceName);
}

ID GLGPUAssets::createTexture(std::string resourceName, ResourceHandle<ImageResource>&& imageHandle)
{
	auto glTextureHandle = m_resourceHub.getManager<GLTexture>().addResource(
		GLTexture(resourceName, std::move(imageHandle))
	);
	return glTextureHandle.getResource()->getInstanceID();
}

} // Engine