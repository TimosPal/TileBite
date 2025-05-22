#ifndef GLGPU_ASSETS_HPP
#define GLGPU_ASSETS_HPP

#include "renderer/IGPUAssets.hpp"
#include "renderer/backend/openGL/GLResourceHub.hpp"

namespace Engine {

class GLGPUAssets : public IGPUAssets {
public:
	GLGPUAssets(GLResourceHub& resourceHub);

	ID getTexture(std::string resourceName) override;
	ID createTexture(std::string resourceName, ResourceHandle<ImageResource>&& imageHandle) override;

private:
	GLResourceHub& m_resourceHub;
};

} // Engine

#endif // !GLGPU_ASSETS_HPP