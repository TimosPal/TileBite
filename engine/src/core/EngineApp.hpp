#ifndef ENGINE_APP_HPP
#define ENGINE_APP_HPP

#include "resources/ResourceManager.hpp"
#include "resources/types/TextFileResource.hpp"
#include "events/Event.hpp"
#include "events/EventQueue.hpp"
#include "events/EventDispatcher.hpp"

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

	void stop() { m_isRunning = false; }

	void onEvent(std::unique_ptr<Event> event);

	template<typename EventType>
	requires DerivedFrom<EventType, Event>
	void subscribe(EventCallback<EventType>& eventCallback)
	{
		m_eventDispatcher.subscribe<EventType>(eventCallback);
	}

	template<typename EventType>
	requires DerivedFrom<EventType, Event>
	void unsubscribe(EventCallback<EventType>& eventCallback)
	{
		m_eventDispatcher.unsubscribe<EventType>(eventCallback);
	}

private:
	static EngineApp* s_instance;

	ResourceManager<TextFileResource> m_textFilesResourceManager;

	EventQueue m_eventQueue;
	EventDispatcher m_eventDispatcher;

	bool m_isRunning;
};

} // Engine

#endif // !ENGINE_APP_HPP
