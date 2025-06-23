#ifndef ASSETS_MANAGER_HPP
#define ASSETS_MANAGER_HPP

#include "core/pch.hpp"
#include "utilities/IDGenerator.hpp"
#include "resources/ResourceHandle.hpp"
#include "resources/types/ImageResource.hpp"
#include "resources/SystemResourceHub.hpp"
#include "renderer/Renderer2D.hpp"
#include "renderer/IGPUAssets.hpp"
#include "ecs/types/EngineComponents.hpp"

namespace Engine {

class AssetsManager {
public:
	void init(SystemResourceHub* resourceHub, IGPUAssets* gpuAssets)
	{
		m_resourceHub = resourceHub;
		m_gpuAssets = gpuAssets;
	}

	std::unique_ptr<IResourceHandle> getTexture(std::string resourceName)
	{
		return m_gpuAssets->getTexture(resourceName + "Texture");
	}

	std::unique_ptr<IResourceHandle> createTexture(std::string resourceName, std::string path)
	{
		// TODO: Split into createImage if needed. (Avoids custom internal Texture sub string)
		auto imageHandle = m_resourceHub->getManager<ImageResource>().addResource(
			ImageResource(resourceName, path)
		);
		
		return m_gpuAssets->createTexture(resourceName + "Texture", std::move(imageHandle));
	}

	ID createTilemap(std::string resourceName, std::vector<Tile> tiles)
	{
		//auto tilemapHandle = m_resourceHub->getManager<TilemapResource>().addResource(
		//	TilemapResource(resourceName, tiles)
		//);

		//// TODO: Temporary logic, make every texture persistent for entire app.
		//m_gpuAssets->makeTexturePersistent(resourceName + "Texture");

		//return resourceID;
	}

private:
	SystemResourceHub* m_resourceHub = nullptr;
	IGPUAssets* m_gpuAssets = nullptr;
};

} // Engine

#endif // !ASSETS_MANAGER_HPP