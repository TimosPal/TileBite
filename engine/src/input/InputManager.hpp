#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include "core/pch.hpp"
#include "utilities/assertions.hpp"
#include <glm/glm.hpp>

namespace TileBite {

class InputManager {
public:
	struct KeyState {
		enum class State {
			RELEASED,
			DOWN,
			REPEAT
		} previous, current;
	};

	// Mouse position
	float getMouseX() const { return m_mouseX; }
	float getMouseY() const { return m_mouseY; }
	void setMousePosition(float x, float y) {
		m_mouseX = x;
		m_mouseY = y;
	}
	glm::vec2 getMousePosition() const {
		return glm::vec2(m_mouseX, m_mouseY);
	}
	
	// Mouse scroll
	float getMouseScrollX() const { return m_mouseScrollX; }
	float getMouseScrollY() const { return m_mouseScrollY; }
	void setMouseScroll(float xOffset, float yOffset) {
		m_mouseScrollX = xOffset;
		m_mouseScrollY = yOffset;
	}

	void setKeyState(int key, KeyState::State newState) {
		auto& state = m_states[key];
		state.current = newState;
	}

	void update() {
		for (auto& [key, state] : m_states)
			state.previous = state.current;
		m_mouseScrollX = 0.0f;
		m_mouseScrollY = 0.0f;
	}

	// Mouse buttons
	bool isMouseButtonPressed(int button) {
		KeyState state = m_states[button];
		return state.current == KeyState::State::DOWN && state.previous == KeyState::State::RELEASED;
	}

	bool isMouseButtonDown(int button) {
		KeyState state = m_states[button];
		return (state.current == KeyState::State::DOWN || state.current == KeyState::State::REPEAT)
			&& (state.previous == KeyState::State::DOWN || state.previous == KeyState::State::REPEAT);
	}

	bool isMouseButtonReleased(int button) {
		KeyState state = m_states[button];
		return state.current == KeyState::State::RELEASED && state.previous == KeyState::State::DOWN;
	}

	// Keyboard keys
	bool isKeyPressed(int keyCode) {
		KeyState state = m_states[keyCode];
		return state.current == KeyState::State::DOWN && state.previous == KeyState::State::RELEASED;
	}

	bool isKeyDown(int keyCode) {
		KeyState state = m_states[keyCode];
		return (state.current == KeyState::State::DOWN || state.current == KeyState::State::REPEAT)
			&& (state.previous == KeyState::State::DOWN || state.previous == KeyState::State::REPEAT);
	}

	bool isKeyRepeat(int keyCode) {
		KeyState state = m_states[keyCode];
		return state.current == KeyState::State::REPEAT;
	}

	bool isKeyReleased(int keyCode) {
		KeyState state = m_states[keyCode];
		return state.current == KeyState::State::RELEASED
			&& (state.previous == KeyState::State::DOWN || state.previous == KeyState::State::REPEAT);
	}

private:
	float m_mouseX = 0.0f;
	float m_mouseY = 0.0f;
	float m_mouseScrollX = 0.0f;
	float m_mouseScrollY = 0.0f;

	std::unordered_map<int, KeyState> m_states;
};

} // TileBite

#endif // !INPUT_MANAGER_HPP
