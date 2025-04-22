#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "core/pch.hpp"

namespace Engine {

class Window {
public:
	struct Data {
		std::string title;
		uint32_t width;
		uint32_t height;
	};

	Window(const Data& data) : m_data(data) {}

	virtual bool init() { return false; }
	virtual bool terminate() { return false; }

	virtual void onUpdate() {}

	static std::unique_ptr<Window> createWindow(const Data& data);
private:
	Data m_data;
};

} // Engine

#endif // !WINDOW_HPP