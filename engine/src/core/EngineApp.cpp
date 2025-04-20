#include "core/EngineApp.hpp"

#include "utilities/Assertions.hpp"

namespace Engine {

EngineApp* EngineApp::s_instance;

EngineApp::EngineApp()
	: m_isRunning(true)
{
	// One instance allowed.
	ASSERT(s_instance == nullptr);
	s_instance = this;
}

void EngineApp::init()
{
	// Application initialization.
}

void EngineApp::run()
{
	// Main Engine loop.
	while (m_isRunning)
	{
		m_eventQueue.dispatchAll(m_eventDispatcher);

	}
}

void EngineApp::terminate()
{
	// Application cleanup.
}

void EngineApp::onEvent(std::unique_ptr<Event> event)
{
	if (event->getIsBlocking())
	{
		// Imediate dispatch
		m_eventDispatcher.dispatch(*event);
	}
	else
	{
		// Push to event queue for batch dispatch.
		m_eventQueue.push(std::move(event));
	}
}

} // Engine