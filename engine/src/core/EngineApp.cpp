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

	}
}

void EngineApp::terminate()
{
	// Application cleanup.
}

} // Engine