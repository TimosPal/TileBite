#ifndef GLGPU_ASSETS_HPP
#define GLGPU_ASSETS_HPP

#include "renderer/IGPUAssets.hpp"
#include "renderer/backend/openGL/GLResourceHub.hpp"

namespace Engine {

class GLGPUAssets : public IGPUAssets {
public:
	GLGPUAssets(GLResourceHub& resourceHub);

	void makeTexturePersistent(std::string resourceName) override;

	ID getTexture(std::string resourceName) override;
	ID createTexture(std::string resourceName, ResourceHandle<ImageResource>&& imageHandle) override;
	
	void clear();

private:
	GLResourceHub& m_resourceHub;

	// TODO: Temporary sollution for persistent renderer texture assets
	// (Avoids constant reloading)
	std::vector<ResourceHandle<GLTexture>> m_persistentTextureHandles;
};

} // Engine

#endif // !GLGPU_ASSETS_HPP