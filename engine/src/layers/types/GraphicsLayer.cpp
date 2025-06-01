#include "layers/types/GraphicsLayer.hpp"

#include "ecs/types/SpriteRenderSystem.hpp"
#include "renderer/Renderer2D.hpp"
#include "events/types/WindowResizeEvent.hpp"

namespace Engine
{

GraphicsLayer::GraphicsLayer(std::shared_ptr<Renderer2D> renderer2D)
	: m_renderer2D(renderer2D)
{}

void GraphicsLayer::onAttach()
{
	// Resize window
	EventCallback<WindowResizeEvent> windowResizeEventCallback([&](WindowResizeEvent& event) {
		event.consume();
		m_renderer2D->setViewportSize(event.getWidth(), event.getHeight());
	});
	subscribe<WindowResizeEvent>(windowResizeEventCallback);

	addSystem(std::make_unique<SpriteRenderSystem>(m_renderer2D));
}

} // Engine