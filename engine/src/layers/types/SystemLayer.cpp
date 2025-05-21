#include "layers/types/SystemLayer.hpp"

#include "events/types/WindowCloseEvent.hpp"
#include "events/types/WindowResizeEvent.hpp"
#include "events/EventCallback.hpp"

namespace Engine {

SystemLayer::SystemLayer(std::function<void()> stopAppCallback, World& world, AssetsManager& assetsManager)
	: Layer(world, assetsManager), m_stopAppCallback(stopAppCallback)
{}

void SystemLayer::onAttach()
{
	// Close window
	EventCallback<WindowCloseEvent> windowCloseEventCallback([&](WindowCloseEvent& event) {
		event.consume();
		m_stopAppCallback();
	});
	subscribe<WindowCloseEvent>(windowCloseEventCallback);

	// Resize window
	EventCallback<WindowResizeEvent> windowResizeEventCallback([&](WindowResizeEvent& event) {
		event.consume();
		// TODO: glViewport(0, 0, width, height);  // Update OpenGL viewport size
	});
	subscribe<WindowResizeEvent>(windowResizeEventCallback);
}

} // Engine