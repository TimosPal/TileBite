#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "core/pch.hpp"
#include "events/Event.hpp"

namespace Engine {

class Window {
public:
	struct Data {
		uint32_t width;
		uint32_t height;
		std::string title;

		std::function<void(std::unique_ptr<Event>)> onEvent;
	};

	Window(const Data& data) : m_data(data) {}

	virtual bool init() { return false; }
	virtual bool terminate() { return false; }

	virtual void pollEvents() {}
	virtual void swapBuffers() {}

	static std::unique_ptr<Window> createWindow(const Data& data);
protected:
	Data m_data;
};

} // Engine

#endif // !WINDOW_HPP