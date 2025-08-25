#include "layers/types/GraphicsLayer.hpp"

#include "ecs/types/SpriteRenderSystem.hpp"
#include "ecs/types/TilemapRenderSystem.hpp"
#include "renderer/Renderer2D.hpp"
#include "events/types/WindowResizeEvent.hpp"

namespace TileBite
{

GraphicsLayer::GraphicsLayer()
	: Layer(getName())
{}

void GraphicsLayer::onAttach()
{
	// Resize window
	EventCallback<WindowResizeEvent> windowResizeEventCallback([&](WindowResizeEvent& event) {
		event.consume();
		getRenderer().setViewportSize(event.getWidth(), event.getHeight());
	});
	getEventDispatcher().subscribe<WindowResizeEvent>(windowResizeEventCallback);

	getSystemManager().addSystem(std::make_unique<SpriteRenderSystem>());
	getSystemManager().addSystem(std::make_unique<TilemapRenderSystem>());
}

} // TileBite