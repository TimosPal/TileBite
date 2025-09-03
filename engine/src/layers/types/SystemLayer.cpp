#include "layers/types/SystemLayer.hpp"

#include "events/types/WindowCloseEvent.hpp"
#include "events/types/WindowResizeEvent.hpp"
#include "events/EventCallback.hpp"
#include "events/types/KeyEvent.hpp"
#include "events/types/MouseEvent.hpp"

#include "input/InputManager.hpp"
#include "ecs/types/CollidersUpdateSystem.hpp"
#include "ecs/types/HierarchiesUpdateSystem.hpp"

namespace TileBite {

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

	// Resize window
	EventCallback<WindowResizeEvent> windowResizeEventCallback([&](WindowResizeEvent& event) {
		float aspect = (float)event.getWidth() / event.getHeight();
		getSceneManager().getActiveScene()->getCameraController()->setProjection(
			-aspect, aspect, -1.0f, 1.0f
		);
	});
	getEventDispatcher().subscribe<WindowResizeEvent>(windowResizeEventCallback);

	InputManager& inputManager = getInputManager();

	// Key events
	EventCallback<KeyPressedEvent> keyPressedEventCallback([&](KeyPressedEvent& event) {
		inputManager.setKeyState(event.getKeyCode(), InputManager::KeyState::State::DOWN);
	});

	EventCallback<KeyReleasedEvent> keyReleasedEventCallback([&](KeyReleasedEvent& event) {
		inputManager.setKeyState(event.getKeyCode(), InputManager::KeyState::State::RELEASED);
	});

	EventCallback<KeyRepeatEvent> keyRapeatEventCallback([&](KeyRepeatEvent& event) {
		inputManager.setKeyState(event.getKeyCode(), InputManager::KeyState::State::REPEAT);
	});


	// Mouse events
	EventCallback<MouseKeyPressedEvent> mouseButtonPressedEventCallback([&](MouseKeyPressedEvent& event) {
		inputManager.setKeyState(event.getKeyCode(), InputManager::KeyState::State::DOWN);
	});

	EventCallback<MouseKeyReleasedEvent> mouseButtonReleasedEventCallback([&](MouseKeyReleasedEvent& event) {
		inputManager.setKeyState(event.getKeyCode(), InputManager::KeyState::State::DOWN);
	});

	EventCallback<MouseMovementEvent> mouseMovedEventCallback([&](MouseMovementEvent& event) {
		inputManager.setMousePosition(event.getX(), event.getY());
	});

	EventCallback<MouseScrollEvent> mouseScrollEventCallback([&](MouseScrollEvent& event) {
		inputManager.setMouseScroll(event.getXOffset(), event.getYOffset());
	});

	EventDispatcher& eventDispatcher = getEventDispatcher();
	eventDispatcher.subscribe(windowCloseEventCallback);
	eventDispatcher.subscribe(keyPressedEventCallback);
	eventDispatcher.subscribe(keyReleasedEventCallback);
	eventDispatcher.subscribe(keyRapeatEventCallback);
	eventDispatcher.subscribe(mouseButtonPressedEventCallback);
	eventDispatcher.subscribe(mouseButtonReleasedEventCallback);
	eventDispatcher.subscribe(mouseMovedEventCallback);
	eventDispatcher.subscribe(mouseScrollEventCallback);

	getSystemManager().addSystem(std::make_unique<HierarchiesUpdateSystem>());
	getSystemManager().addSystem(std::make_unique<ColliderUpdateSystem>());
}

} // TileBite