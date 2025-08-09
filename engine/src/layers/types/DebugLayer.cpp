#include "layers/types/DebugLayer.hpp"

#include "renderer/Renderer2D.hpp"
#include "ecs/types/ColliderRenderSystem.hpp"

namespace Engine
{

DebugLayer::DebugLayer()
	: Layer(getName()) {}

void DebugLayer::onAttach()
{
	getSystemManager().addSystem(std::make_unique<ColliderRenderSystem>());
}

} // Engine