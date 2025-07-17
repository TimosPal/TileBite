#include "layers/types/DebugLayer.hpp"

#include "renderer/Renderer2D.hpp"
#include "ecs/types/ColliderRenderSystem.hpp"

namespace Engine
{

DebugLayer::DebugLayer(std::shared_ptr<Renderer2D> renderer2D)
	: Layer(getName()),
	m_renderer2D(renderer2D)
{}

void DebugLayer::onAttach()
{
	getSystemManager().addSystem(std::make_unique<ColliderRenderSystem>(m_renderer2D));
}

} // Engine