#ifndef GLFW_WINDOW_HPP  
#define GLFW_WINDOW_HPP  

#include <GLFW/glfw3.h>

#include "window/Window.hpp"

namespace Engine {  

class GlfwWindow : public Window {  
public:  
   GlfwWindow(const Data& data);  

   bool init() override;  
   bool terminate() override;  

   void onUpdate() override;  
private:  
   GLFWwindow* m_glfwWindow = nullptr;  
};  

} // Engine  

#endif // !GLFW_WINDOW_HPP