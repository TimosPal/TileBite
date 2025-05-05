#include "window/backend/glfw/GlfwWindow.hpp"

#include "utilities/Logger.hpp"
#include "events/types/WindowCloseEvent.hpp"
#include "events/types/WindowResizeEvent.hpp"

namespace Engine {

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

	// * ======================= *
	// * Window callbacks setup  *
	// * ======================= *
	
	// Close            
	glfwSetWindowCloseCallback(m_glfwWindow, [](GLFWwindow* window) {
		Data* data = static_cast<Data*>(glfwGetWindowUserPointer(window));
		data->onEvent(std::make_unique<WindowCloseEvent>());
	});
	
	// Resize window
	glfwSetFramebufferSizeCallback(m_glfwWindow, [](GLFWwindow* window, int width, int height) {
		Data* data = static_cast<Data*>(glfwGetWindowUserPointer(window));
		data->width = width;
		data->height = height;
		data->onEvent(std::make_unique<WindowResizeEvent>(WindowResizeEvent(width, height)));
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


} // Engine