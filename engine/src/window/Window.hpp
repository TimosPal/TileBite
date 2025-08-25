#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "core/pch.hpp"
#include "events/Event.hpp"

namespace TileBite {

class Window {
public:
	struct Data {
		uint32_t width;
		uint32_t height;
		std::string title;

		std::function<void(std::unique_ptr<Event>)> pushEvent;
	};

	Window(const Data& data) : m_data(data) {}

	virtual bool init() { return false; }
	virtual bool terminate() { return false; }

	virtual void pollEvents() {}
	virtual void swapBuffers() {}

	uint32_t getWidth() const { return m_data.width; }
	uint32_t getHeight() const { return m_data.height; }
	std::string getTitle() const { return m_data.title; }

	static std::shared_ptr<Window> createWindow(const Data& data);
protected:
	Data m_data;
};

} // TileBite

#endif // !WINDOW_HPP