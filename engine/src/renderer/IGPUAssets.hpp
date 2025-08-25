#ifndef IGPU_ASSETS_HPP
#define IGPU_ASSETS_HPP

#include "core/pch.hpp"
#include "utilities/IDGenerator.hpp"
#include "resources/ResourceHandle.hpp"
#include "resources/types/ImageResource.hpp"

namespace TileBite {

// Bridge communication between AssetsManager and GPU (API specific) resource management
// Used for client side asset creation, not for internal use. Returns IDs not ResourceHandles.
class IGPUAssets {
public:
	virtual void makeTexturePersistent(std::string resourceName) = 0;
	virtual std::unique_ptr<IResourceHandle> getTextureResource(std::string resourceName) = 0;
	virtual std::unique_ptr<IResourceHandle> createTextureResource(std::string resourceName, ResourceHandle<ImageResource>&& imageHandle) = 0;
};

} // TileBite

#endif // !IGPU_ASSETS_HPP