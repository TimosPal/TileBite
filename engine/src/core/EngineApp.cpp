#include "core/EngineApp.hpp"

#include "utilities/assertions.hpp"
#include "layers/types/SystemLayer.hpp"
#include "layers/types/GraphicsLayer.hpp"

namespace Engine {

EngineApp* EngineApp::s_instance;

EngineApp::EngineApp()
	: m_isRunning(true)
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

	// Load engine resources
	// Needs to be done before renderer since it needs to load shaders.
	bool resResourceHubInit = m_resourceHub.init();
	ASSERT(resResourceHubInit, "Resource hub not init");

	// Renderer creation.
	m_renderer2D = Renderer2D::createRenderer2D(m_resourceHub);
	bool resInitRendered2D = m_renderer2D->init();
	ASSERT(resInitRendered2D, "Renderer2D not init");

	// Engine layers creation.
	auto stopAppCallback = [&]() { stop(); };
	pushLayer(std::make_unique<SystemLayer>(SystemLayer(stopAppCallback, m_world)));
	pushOverlay(std::make_unique<GraphicsLayer>(GraphicsLayer(m_world, m_renderer2D)));

	LOG_INFO("Engine init successfully");
}

void EngineApp::run()
{
	using Clock = std::chrono::high_resolution_clock;

	// Engine loop.
	float deltaTime = 0.0f;
	auto lastFrameTime = Clock::now();
	while (m_isRunning)
	{
		auto currentFrameTime = Clock::now();
		deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();
		lastFrameTime = currentFrameTime;

		m_window->pollEvents();
		m_eventQueue.dispatchAll(m_layers);
		
		m_renderer2D->clearScreen();

		m_layers.onUpdate(deltaTime);

		m_renderer2D->render();
		m_window->swapBuffers();

		static int i = 0;
		if(i++ % 40 == 0)
			LOG_INFO("FPS: {}", 1.0f / deltaTime);
	}
}

void EngineApp::terminate()
{
	// Window cleanup
	bool resTerminateWindow = m_window->terminate();
	ASSERT(resTerminateWindow, "Window not terminated");

	// Renderer2D cleanup
	bool resTerminateRenderer2D = m_renderer2D->terminate();
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

} // Engine