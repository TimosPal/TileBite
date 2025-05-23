#ifndef ASSETS_MANAGER_HPP
#define ASSETS_MANAGER_HPP

#include "core/pch.hpp"
#include "utilities/IDGenerator.hpp"
#include "resources/ResourceHandle.hpp"
#include "resources/types/ImageResource.hpp"
#include "resources/SystemResourceHub.hpp"
#include "renderer/Renderer2D.hpp"
#include "renderer/IGPUAssets.hpp"

namespace Engine {

class AssetsManager {
public:
	void init(SystemResourceHub* resourceHub, IGPUAssets* gpuAssets)
	{
		m_resourceHub = resourceHub;
		m_gpuAssets = gpuAssets;
	}

	ID getTexture(std::string resourceName)
	{
		return m_gpuAssets->getTexture(resourceName + "Texture");
	}

	ID createTexture(std::string resourceName, std::string path)
	{
		// TODO: Split into createImage if needed. (Avoids custom internal Texture sub string)
		auto imageHandle = m_resourceHub->getManager<ImageResource>().addResource(
			ImageResource(resourceName, path)
		);
		ID resourceID = m_gpuAssets->createTexture(resourceName + "Texture", std::move(imageHandle));

		// TODO: Temporary logic, make every texture persistent for entire app.
		m_gpuAssets->makeTexturePersistent(resourceName + "Texture");

		return resourceID;
	}

private:
	SystemResourceHub* m_resourceHub = nullptr;
	IGPUAssets* m_gpuAssets = nullptr;
};

} // Engine

#endif // !ASSETS_MANAGER_HPP