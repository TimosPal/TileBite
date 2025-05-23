#include "GLGPUAssets.hpp"
#include "renderer/backend/openGL/GLGPUAssets.hpp"

namespace Engine {

GLGPUAssets::GLGPUAssets(GLResourceHub& resourceHub) : m_resourceHub(resourceHub) {}

void GLGPUAssets::makeTexturePersistent(std::string resourceName)
{
	auto handle = m_resourceHub.getManager<GLTexture>().getResource(resourceName);
	handle.watch();
	handle.load();

	// Storing an active handle will keep them persistent durring time.
	// (Also prevents runtime stalls after initial loads)
	m_persistentTextureHandles.emplace_back(std::move(handle));
}

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

void GLGPUAssets::clear()
{
	for (auto& handle : m_persistentTextureHandles) {
		handle.unwatch();
		handle.unload();
	}
}

} // Engine