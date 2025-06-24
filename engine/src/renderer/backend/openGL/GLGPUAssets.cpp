#include "GLGPUAssets.hpp"
#include "renderer/backend/openGL/GLGPUAssets.hpp"

namespace Engine {

GLGPUAssets::GLGPUAssets(GLResourceHub& resourceHub) : m_resourceHub(resourceHub) {}

void GLGPUAssets::makeTexturePersistent(std::string resourceName)
{
	auto handle = m_resourceHub.getManager<GLTexture>().getResource(resourceName);
	handle.watch();
	handle.load();

	// Only to be used by internal textures
	// Storing an active handle will keep them persistent durring run time.
	// (Also prevents runtime stalls after initial loads)
	m_persistentTextureHandles.emplace_back(std::move(handle));
}

std::unique_ptr<IResourceHandle> GLGPUAssets::getTextureResource(std::string resourceName)
{
	return std::make_unique<ResourceHandle<GLTexture>>(m_resourceHub.getManager<GLTexture>().getResource(resourceName));
}

std::unique_ptr<IResourceHandle> GLGPUAssets::createTextureResource(std::string resourceName, ResourceHandle<ImageResource>&& imageHandle)
{
	auto glTextureHandle = m_resourceHub.getManager<GLTexture>().addResource(
		GLTexture(resourceName, std::move(imageHandle))
	);

	return std::make_unique<ResourceHandle<GLTexture>>(std::move(glTextureHandle));
}

void GLGPUAssets::clear()
{
	for (auto& handle : m_persistentTextureHandles) {
		handle.unwatch();
		handle.unload();
	}
}

} // Engine