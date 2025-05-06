#include "layers/types/GraphicsLayer.hpp"

#include "ecs/types/SpriteRenderSystem.hpp"

namespace Engine
{

GraphicsLayer::GraphicsLayer(World& world) : Layer(world)
{}

void GraphicsLayer::onAttach()
{
	addSystem(std::make_unique<SpriteRenderSystem>());
}

} // Engine