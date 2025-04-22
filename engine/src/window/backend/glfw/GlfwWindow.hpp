#ifndef GLFW_WINDOW_HPP
#define GLFW_WINDOW_HPP

#include "window/Window.hpp"

namespace Engine {

class GlfwWindow : public Window {
public:
	GlfwWindow(const Data& data);

	bool init() override;
	bool terminate() override;

	void onUpdate() override;
};

} // Engine

#endif // !GLFW_WINDOW_HPP