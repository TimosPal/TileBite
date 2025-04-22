#include "window/backend/glfw/GlfwWindow.hpp"

#include "utilities/Logger.hpp"

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

	m_glfwWindow = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), NULL, NULL);
	if (!m_glfwWindow)
	{
		LOG_CRITICAL("Failed to create GLFW window");
		return false;
	}

	glfwMakeContextCurrent(m_glfwWindow);

	return true; 
}

bool GlfwWindow::terminate() 
{
	glfwDestroyWindow(m_glfwWindow);
	glfwTerminate();

	return true;
}

void GlfwWindow::onUpdate() 
{

}

} // Engine