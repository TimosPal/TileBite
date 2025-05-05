#include "core/EngineApp.hpp"

#include "utilities/assertions.hpp"
#include "layers/types/SystemLayer.hpp"

namespace Engine {

EngineApp* EngineApp::s_instance;

EngineApp::EngineApp()
	: m_isRunning(true), m_systemManager(m_world)
{
	// One instance allowed.
	ASSERT(s_instance == nullptr, "Engine app already created");
	s_instance = this;
}

void EngineApp::init()
{
	// Application initialization.
	// Window creation.
	Window::Data data = config();
	data.onEvent = [&](std::unique_ptr<Event> event) { onEvent(std::move(event)); };
	m_window = Window::createWindow(data);
	ASSERT(m_window != nullptr, "Window not created");
	bool resInitWindow = m_window->init();
	ASSERT(resInitWindow, "Window not init");

	// Renderer creation.
	m_rendered2D = Renderer2D::createRenderer2D();
	bool resInitRendered2D = m_rendered2D->init();
	ASSERT(resInitRendered2D, "Renderer2D not init");

	// Engine layers creation.
	auto stopAppCallback = [&]() { stop(); };
	pushLayer(std::make_unique<SystemLayer>(SystemLayer(stopAppCallback)));
}

void EngineApp::run()
{
	// Engine loop.
	float deltaTime = 0.0f; // TODO: calculate.
	while (m_isRunning)
	{
		m_window->pollEvents();
		m_eventQueue.dispatchAll(m_layers);

		m_systemManager.updateSystems(deltaTime);
		
		m_rendered2D->clearScreen();
		m_rendered2D->render();
		m_window->swapBuffers();
	}
}

void EngineApp::terminate()
{
	// Window cleanup
	bool resTerminateWindow = m_window->terminate();
	ASSERT(resTerminateWindow, "Window not terminated");

	// Renderer2D cleanup
	bool resTerminateRenderer2D = m_rendered2D->terminate();
	ASSERT(resTerminateRenderer2D, "Renderer2D not terminated");
}

void EngineApp::onEvent(std::unique_ptr<Event> event)
{
	if (event->getIsBlocking())
	{
		// Imediate dispatch
		m_layers.dispatchEventToLayers(*event);
	}
	else
	{
		// Push to event queue for batch dispatch.
		m_eventQueue.push(std::move(event));
	}
}

void EngineApp::pushLayer(std::unique_ptr<Layer> layer)
{
	m_layers.pushLayer(std::move(layer));
}

void EngineApp::pushOverlay(std::unique_ptr<Layer> layer)
{
	m_layers.pushOverlay(std::move(layer));
}

void EngineApp::addSystem(std::unique_ptr<ISystem> system)
{
	m_systemManager.addSystem(std::move(system));
}

} // Engine