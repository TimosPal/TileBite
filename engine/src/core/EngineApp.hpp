#ifndef ENGINE_APP_HPP
#define ENGINE_APP_HPP

#include "core/AppConfig.hpp"
#include "resources/ResourceManager.hpp"
#include "resources/types/TextFileResource.hpp"
#include "events/Event.hpp"
#include "events/EventQueue.hpp"
#include "events/EventDispatcher.hpp"
#include "layers/LayerStack.hpp"
#include "window/Window.hpp"
#include "ecs/SystemManager.hpp"
#include "ecs/World.hpp"

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

	void addSystem(std::unique_ptr<ISystem> system);

	World& getWorld() { return m_world; }

private:
	static EngineApp* s_instance;

	std::unique_ptr<Window> m_window;

	ResourceManager<TextFileResource> m_textFilesResourceManager;

	// Layer ID -> EventDispatcher.
	EventQueue m_eventQueue;
	LayerStack m_layers;

	// ECS
	World m_world;
	SystemManager m_systemManager;

	bool m_isRunning;
};

} // Engine

#endif // !ENGINE_APP_HPP
