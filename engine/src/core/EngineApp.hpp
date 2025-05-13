#ifndef ENGINE_APP_HPP
#define ENGINE_APP_HPP

#include "core/AppConfig.hpp"
#include "events/Event.hpp"
#include "events/EventQueue.hpp"
#include "events/EventDispatcher.hpp"
#include "layers/LayerStack.hpp"
#include "window/Window.hpp"
#include "ecs/World.hpp"
#include "renderer/Renderer2D.hpp"
#include "resources/types/SystemResourceHub.hpp"

namespace Engine {

class EngineApp {
public:
	static EngineApp* getInstance() { return s_instance; }

	EngineApp();

	void init();
	void run();
	void terminate();
	
	// * =============================== *
	// * User side setup implementation. *
	// * =============================== *
	virtual void setup() = 0;
	// Override if needing custom config.
	virtual AppConfig config() { return AppConfig(); }

	void stop() { m_isRunning = false; }

	void onEvent(std::unique_ptr<Event> event);

	void pushLayer(std::unique_ptr<Layer> layer);
	void pushOverlay(std::unique_ptr<Layer> layer);

	World& getWorld() { return m_world; }

private:
	static EngineApp* s_instance;

	// Resource hub.
	SystemResourceHub m_resourceHub;

	std::unique_ptr<Window> m_window;
	std::shared_ptr<Renderer2D> m_renderer2D;

	// Layer ID -> EventDispatcher.
	EventQueue m_eventQueue;
	LayerStack m_layers;

	// ECS
	World m_world;

	bool m_isRunning;
};

} // Engine

#endif // !ENGINE_APP_HPP
