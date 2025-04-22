#include "window/backend/glfw/GlfwWindow.hpp"

namespace Engine {

GlfwWindow::GlfwWindow(const Data& data) : Window(data) {}

bool GlfwWindow::init() 
{ 
	return true; 
}

bool GlfwWindow::terminate() 
{
	return true;
}

void GlfwWindow::onUpdate() 
{

}

} // Engine