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

namespace TileBite {

// IResourceHandle is used for platform specific implementations.
// eg: Textures can be GLTextures or any custom graphics API implementation
// whereas tilemaps use an engine defined type

class AssetsManager {
public:
	void init(SystemResourceHub* resourceHub, IGPUAssets* gpuAssets)
	{
		m_resourceHub = resourceHub;
		m_gpuAssets = gpuAssets;
	}

	std::unique_ptr<IResourceHandle> createTextureResource(std::string resourceName, std::string path)
	{
		// TODO: Split into createImage if needed. (Avoids custom internal Texture sub string)
		auto imageHandle = m_resourceHub->getManager<ImageResource>().addResource(
			ImageResource(resourceName, path)
		);
		
		return m_gpuAssets->createTextureResource(resourceName + "Texture", std::move(imageHandle));
	}

	std::unique_ptr<IResourceHandle> getTextureResource(std::string resourceName)
	{
		return m_gpuAssets->getTextureResource(resourceName + "Texture");
	}

	// TODO: refactor
	ResourceHandle<TilemapResource> createTilemapResource(
		std::string resourceName,
		std::vector<Tile> tiles,
		glm::vec2 dimensions,
		glm::vec2 worldTileSize,
		glm::vec2 atlasTileSize,
		glm::vec2 atlasDim,
		ID atladID)
	{
		auto tilemapHandle = m_resourceHub->getManager<TilemapResource>().addResource(
			TilemapResource(resourceName, tiles, dimensions, worldTileSize, atlasTileSize, atlasDim, atladID)
		);

		return tilemapHandle;
	}

	ResourceHandle<TilemapResource> getTilemapResource(std::string resourceName)
	{
		return m_resourceHub->getManager<TilemapResource>().getResource(resourceName);
	}

	ResourceHandle<TilemapResource> getTilemapResource(ID resourceID)
	{
		return m_resourceHub->getManager<TilemapResource>().getResource(resourceID);
	}

private:
	SystemResourceHub* m_resourceHub = nullptr;
	IGPUAssets* m_gpuAssets = nullptr;
};

} // TileBite

#endif // !ASSETS_MANAGER_HPP