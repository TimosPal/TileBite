#include "layers/types/GraphicsLayer.hpp"

#include "ecs/types/SpriteRenderSystem.hpp"
#include "renderer/Renderer2D.hpp"

namespace Engine
{

GraphicsLayer::GraphicsLayer(World& world, std::shared_ptr<Renderer2D> renderer2D)
	: Layer(world), m_renderer2D(renderer2D)
{}

void GraphicsLayer::onAttach()
{
	addSystem(std::make_unique<SpriteRenderSystem>(m_renderer2D));
}

} // Engine