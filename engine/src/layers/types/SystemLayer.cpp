#include "layers/types/SystemLayer.hpp"

namespace Engine {

SystemLayer::SystemLayer(std::function<void()> stopAppCallback) : m_stopAppCallback(stopAppCallback) {}

void SystemLayer::onAttach()
{
	EventCallback<WindowCloseEvent> windowCloseEventCallback([&](WindowCloseEvent& event) {
		event.consume();
		m_stopAppCallback();
	});
	subscribe<WindowCloseEvent>(windowCloseEventCallback);
}

} // Engine