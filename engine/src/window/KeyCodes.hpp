#ifndef KEYCODES_HPP
#define KEYCODES_HPP

namespace Engine {
namespace KeyCodes {

#if defined(WINDOW_BACKEND_GLFW)
	#include "window/backend/glfw/GLFWKeyCodes.hpp"
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

} // KeyCodes
} // Engine

#endif // !KEYCODES_HPP
