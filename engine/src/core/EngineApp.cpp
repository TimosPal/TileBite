#include "core/EngineApp.hpp"

#include "utilities/assertions.hpp"
#include "renderer/Camera/OrthographicCamera.hpp"
#include <events/types/KeyEvent.hpp>
#include <window/KeyCodes.hpp>

namespace TileBite {

EngineApp* EngineApp::s_instance;

EngineApp::EngineApp()
	: m_isRunning(true),
	m_sceneManager()
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
	data.pushEvent = [&](std::unique_ptr<Event> event) { pushEvent(std::move(event)); };
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
	
	// Assets interface
	m_assetsManager.init(&m_resourceHub, &m_renderer2D->getGPUAssets());

	// Engine layers creation.
	auto stopAppCallback = [&]() { stop(); };

	pushLayer(std::make_shared<SystemLayer>(SystemLayer(stopAppCallback)));
	pushOverlay(std::make_shared<GraphicsLayer>(GraphicsLayer()));
	auto debugLayer = std::make_shared<DebugLayer>(DebugLayer());
	debugLayer->disable(); // Disable by default, enable it in setup.
	pushOverlay(debugLayer);

	// Event queue setup. (Order matters)
	m_eventQueue.addEventHandler([&](Event& event) {
		// Dispatch to layers.
		m_layers.dispatchEventToLayers(event);
	});
	m_eventQueue.addEventHandler([&](Event& event) {
		// Dispatch to core dispatcher.
		m_coreEventDispatcher.dispatch(event);
	});

	// Debug layer toggle. (F1 key)
	EventCallback<KeyPressedEvent> debugToggleCallback([&](KeyPressedEvent& event) {
		if(event.getKeyCode() != KeyCodes::KEY_F1) return;

		auto layer = getLayer(DebugLayer::getName());
		if (!layer->isEnabled()) layer->enable();
		else layer->disable();
	});
	m_coreEventDispatcher.subscribe(debugToggleCallback);

	// Graphics layer toggle. (F2 key)
	EventCallback<KeyPressedEvent> graphicsToggleCallback([&](KeyPressedEvent& event) {
		if (event.getKeyCode() != KeyCodes::KEY_F2) return;

		auto layer = getLayer(GraphicsLayer::getName());
		if (!layer->isEnabled()) layer->enable();
		else layer->disable();
	});
	m_coreEventDispatcher.subscribe(graphicsToggleCallback);

	LOG_INFO("############################");
	LOG_INFO("# Engine init successfully #");
	LOG_INFO("############################");
	LOG_INFO("");
}

void EngineApp::run()
{
	using Clock = std::chrono::high_resolution_clock;

	float fpsLogTimer = 0.0f;
	int framesCounter = 0;
	
	// Engine loop.
	float deltaTime = 0.0f;
	auto lastFrameTime = Clock::now();

	while (m_isRunning)
	{
		auto currentFrameTime = Clock::now();
		deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();
		lastFrameTime = currentFrameTime;

		fpsLogTimer += deltaTime;
		framesCounter++;

		m_inputManager.update();

		m_window->pollEvents();
		m_eventQueue.dispatchAll(m_layers);
		
		m_renderer2D->clearScreen();

		auto activeScene = m_sceneManager.getActiveScene();
		ASSERT(activeScene != nullptr, "Active scene not set");
		activeScene->onUpdate(deltaTime);
		activeScene->updateWorldActions();
		
		// NOTE: this should be done after the scene update to prevent flickering issues.
		// Also, graphics should be relevant to the current frame changes.
		m_layers.onUpdate(deltaTime);

		auto activeSceneCamController = activeScene->getCameraController();
		ASSERT(activeSceneCamController != nullptr, "Main camera not set");
		m_renderer2D->render(*activeSceneCamController);
		m_window->swapBuffers();

		if(fpsLogTimer >= 1.0f)
		{
			LOG_INFO("FPS: {}", framesCounter);
			fpsLogTimer = 0;
			framesCounter = 0;
		}
	}
}

void EngineApp::terminate()
{
	LOG_INFO("");
	LOG_INFO("########################");
	LOG_INFO("# Engine shutting down #");
	LOG_INFO("########################");
	LOG_INFO("");

	// Terminate scenes so resources are freed
	m_sceneManager.clearScenes();

	// Renderer2D cleanup
	// Internaly destroys the resource of the renderer.
	bool resTerminateRenderer2D = m_renderer2D->terminate();
	ASSERT(resTerminateRenderer2D, "Renderer2D not terminated");

	// System resource hub
	bool destroyedResourceHub = m_resourceHub.destroy();
	ASSERT(destroyedResourceHub, "System resource hub not terminated");

	// Window cleanup
	bool resTerminateWindow = m_window->terminate();
	ASSERT(resTerminateWindow, "Window not terminated");
}

void EngineApp::pushEvent(std::unique_ptr<Event> event)
{
	if (event->getIsBlocking())
	{
		// Imediate dispatch
		m_layers.dispatchEventToLayers(*event);
		m_coreEventDispatcher.dispatch(*event);
	}
	else
	{
		// Push to event queue for batch dispatch.
		m_eventQueue.push(std::move(event));
	}
}

void EngineApp::pushLayer(std::shared_ptr<Layer> layer)
{
	layer->init();

	m_layers.pushLayer(layer);
}

void EngineApp::pushOverlay(std::shared_ptr<Layer> layer)
{
	layer->init();

	m_layers.pushOverlay(layer);
}

} // TileBite