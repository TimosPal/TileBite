#ifndef APP_CONFIG_HPP
#define APP_CONFIG_HPP

#include "core/pch.hpp"
#include "window/Window.hpp"

namespace TileBite {

struct AppConfig {
	uint32_t width = 800;
	uint32_t height = 600;
	std::string title = "App";

	operator Window::Data() const {
		return Window::Data{width, height, title};
	}
};

} // TileBite

#endif // !APP_CONFIG_HPP
