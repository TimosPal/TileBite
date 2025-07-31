#include "layers/types/SystemLayer.hpp"

#include "events/types/WindowCloseEvent.hpp"
#include "events/types/WindowResizeEvent.hpp"
#include "events/EventCallback.hpp"
#include "events/types/KeyEvent.hpp"
#include "events/types/MouseEvent.hpp"

#include "ecs/types/CollidersUpdateSystem.hpp"

namespace Engine {

SystemLayer::SystemLayer(std::function<void()> stopAppCallback)
	: Layer(getName()),
	m_stopAppCallback(stopAppCallback)
{}

void SystemLayer::onAttach()
{
	// Close window
	EventCallback<WindowCloseEvent> windowCloseEventCallback([&](WindowCloseEvent& event) {
		event.consume();
		m_stopAppCallback();
	});
	getEventDispatcher().subscribe(windowCloseEventCallback);


	// Key events
	EventCallback<KeyPressedEvent> keyPressedEventCallback([&](KeyPressedEvent& event) {
		getInputManager();
	});

	EventCallback<KeyReleasedEvent> keyReleasedEventCallback([&](KeyReleasedEvent& event) {
		getInputManager();
	});

	EventCallback<KeyRepeatEvent> keyTypedEventCallback([&](KeyRepeatEvent& event) {
		getInputManager();
	});

	getEventDispatcher().subscribe(keyPressedEventCallback);
	getEventDispatcher().subscribe(keyReleasedEventCallback);
	getEventDispatcher().subscribe(keyTypedEventCallback);


	// Mouse events
	EventCallback<MouseKeyPressedEvent> mouseButtonPressedEventCallback([&](MouseKeyPressedEvent& event) {
		getInputManager();
	});

	EventCallback<MouseMovementEvent> mouseMovedEventCallback([&](MouseMovementEvent& event) {
		getInputManager();
	});

	getEventDispatcher().subscribe(mouseButtonPressedEventCallback);
	getEventDispatcher().subscribe(mouseMovedEventCallback);


	getSystemManager().addSystem(std::make_unique<ColliderUpdateSystem>());
}

} // Engine