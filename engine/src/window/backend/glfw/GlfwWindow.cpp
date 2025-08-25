#include "window/backend/glfw/GlfwWindow.hpp"

#include "utilities/Logger.hpp"
#include "events/types/WindowCloseEvent.hpp"
#include "events/types/WindowResizeEvent.hpp"
#include "events/types/KeyEvent.hpp"
#include "events/types/MouseEvent.hpp"

namespace TileBite {

GlfwWindow::GlfwWindow(const Data& data) : Window(data) {}

bool GlfwWindow::init() 
{ 
	if (!glfwInit())
	{
		LOG_CRITICAL("Failed to initialize GLFW");
		return false;
	}

	glfwSetErrorCallback(glfwSetErrorCallback([](int code, const char* msg) {
		LOG_ERROR("GLFW Error {}: {}", code, std::string(msg));
	}));

#if defined(RENDERER_OPENGL)
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

	m_glfwWindow = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), NULL, NULL);
	if (!m_glfwWindow)
	{
		LOG_CRITICAL("Failed to create GLFW window");
		return false;
	}
	LOG_INFO("Created GLFW window");

	glfwMakeContextCurrent(m_glfwWindow);
	glfwSetWindowUserPointer(m_glfwWindow, &m_data);

	glfwSwapInterval(0); // gsync off

	// * ======================= *
	// * Window callbacks setup  *
	// * ======================= *
	
	// Close            
	glfwSetWindowCloseCallback(m_glfwWindow, [](GLFWwindow* window) {
		Data* data = static_cast<Data*>(glfwGetWindowUserPointer(window));
		data->pushEvent(std::make_unique<WindowCloseEvent>());
	});

	// Resize window
	glfwSetFramebufferSizeCallback(m_glfwWindow, [](GLFWwindow* window, int width, int height) {
		Data* data = static_cast<Data*>(glfwGetWindowUserPointer(window));
		data->width = width;
		data->height = height;
		data->pushEvent(std::make_unique<WindowResizeEvent>(WindowResizeEvent(width, height)));
	});

	// Key events
	glfwSetKeyCallback(m_glfwWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		Data* data = static_cast<Data*>(glfwGetWindowUserPointer(window));

		switch (action) {
		case GLFW_PRESS: {
			KeyPressedEvent keyPressedEvent(key);
			data->pushEvent(std::make_unique<KeyPressedEvent>(keyPressedEvent));
			break;
		}
		case GLFW_RELEASE: {
			KeyReleasedEvent keyReleasedEvent(key);
			data->pushEvent(std::make_unique<KeyReleasedEvent>(keyReleasedEvent));
			break;
		}
		case GLFW_REPEAT: {
			KeyRepeatEvent keyRepeatEvent(key);
			data->pushEvent(std::make_unique<KeyRepeatEvent>(keyRepeatEvent));
			break;
		}
		}
	});

	// Mouse button events
	glfwSetMouseButtonCallback(m_glfwWindow, [](GLFWwindow* window, int button, int action, int mods) {
		Data* data = static_cast<Data*>(glfwGetWindowUserPointer(window));
		
		switch (action) {
		case GLFW_PRESS: {
			MouseKeyPressedEvent mouseKeyPressedEvent(button);
			data->pushEvent(std::make_unique<MouseKeyPressedEvent>(mouseKeyPressedEvent));
			break;
		}
		case GLFW_RELEASE: {
			MouseKeyReleasedEvent mouseKeyReleasedEvent(button);
			data->pushEvent(std::make_unique<MouseKeyReleasedEvent>(mouseKeyReleasedEvent));
			break;
		}
		}
	});

	// Mouse position event
	glfwSetCursorPosCallback(m_glfwWindow, [](GLFWwindow* window, double xpos, double ypos) {
		Data* data = static_cast<Data*>(glfwGetWindowUserPointer(window));

		MouseMovementEvent mouseMovedEvent(static_cast<float>(xpos), static_cast<float>(ypos));
		data->pushEvent(std::make_unique<MouseMovementEvent>(mouseMovedEvent));
	});

	// Mouse scroll event
	glfwSetScrollCallback(m_glfwWindow, [](GLFWwindow* window, double xoffset, double yoffset) {
		Data* data = static_cast<Data*>(glfwGetWindowUserPointer(window));
		
		MouseScrollEvent mouseScrollEvent(static_cast<float>(xoffset), static_cast<float>(yoffset));
		data->pushEvent(std::make_unique<MouseScrollEvent>(mouseScrollEvent));
	});

	return true; 
}

bool GlfwWindow::terminate() 
{
	glfwDestroyWindow(m_glfwWindow);
	glfwTerminate();
	LOG_INFO("GLFW terminated");

	return true;
}

void GlfwWindow::pollEvents()
{
	glfwPollEvents();
}

void GlfwWindow::swapBuffers()
{
	glfwSwapBuffers(m_glfwWindow);
}


} // TileBite