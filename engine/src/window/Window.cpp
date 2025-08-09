#include "window/Window.hpp"

#if defined(WINDOW_BACKEND_GLFW)
    #include "window/backend/glfw/GlfwWindow.hpp"
    #include "window/backend/glfw/GlfwKeyCodes.hpp"
    using WindowBackend = Engine::GlfwWindow;
#elif defined(WINDOW_BACKEND_NATIVE)
    #if defined(PLATFORM_WINDOWS)
        #error Windows native not supported
    #elif defined(PLATFORM_LINUX)
        #error Linux native not supported
    #elif defined(PLATFORM_MACOS)
        #error MacOS native not supported
    #else
        #error Unknown platform
    #endif
#else
    #error Unknown or unsupported window backend
#endif

namespace Engine {

std::shared_ptr<Window> Window::createWindow(const Data& data)
{
    return std::make_shared<WindowBackend>(data);
}

} // Engine