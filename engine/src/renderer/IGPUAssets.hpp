#ifndef IGPU_ASSETS_HPP
#define IGPU_ASSETS_HPP

#include "core/pch.hpp"
#include "utilities/IDGenerator.hpp"
#include "resources/ResourceHandle.hpp"
#include "resources/types/ImageResource.hpp"

namespace Engine {

// Bridge communication between AssetsManager and GPU (API specific) resource management
// Used for client side asset creation, not for internal use. Returns IDs not ResourceHandles.
class IGPUAssets {
public:
	virtual ID getTexture(std::string resourceName) = 0;
	virtual ID createTexture(std::string resourceName, ResourceHandle<ImageResource>&& imageHandle) = 0;
};

} // Engine

#endif // !IGPU_ASSETS_HPP