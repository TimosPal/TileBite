#ifndef GLGPU_ASSETS_HPP
#define GLGPU_ASSETS_HPP

#include "renderer/IGPUAssets.hpp"
#include "renderer/backend/openGL/GLResourceHub.hpp"

namespace TileBite {

class GLGPUAssets : public IGPUAssets {
public:
	GLGPUAssets(GLResourceHub& resourceHub);

	void makeTexturePersistent(std::string resourceName) override;

	std::unique_ptr<IResourceHandle> getTextureResource(std::string resourceName) override;
	std::unique_ptr<IResourceHandle> createTextureResource(std::string resourceName, ResourceHandle<ImageResource>&& imageHandle) override;
	
	void clear();

private:
	GLResourceHub& m_resourceHub;

	// Persistent renderer texture assets, used for engine resources only!
	// (Avoids constant reloading)
	std::vector<ResourceHandle<GLTexture>> m_persistentTextureHandles;
};

} // TileBite

#endif // !GLGPU_ASSETS_HPP