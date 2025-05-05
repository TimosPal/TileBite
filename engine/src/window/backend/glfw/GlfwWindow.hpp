#ifndef GLFW_WINDOW_HPP  
#define GLFW_WINDOW_HPP  

#include <GLFW/glfw3.h>

#include "window/Window.hpp"

namespace Engine {  

class GlfwWindow : public Window {  
public:  
   GlfwWindow(const Data& data);  

   virtual bool init() override;  
   virtual bool terminate() override;  

   virtual void pollEvents() override;
   virtual void swapBuffers() override;
private:  
   GLFWwindow* m_glfwWindow = nullptr;  
};  

} // Engine  

#endif // !GLFW_WINDOW_HPP