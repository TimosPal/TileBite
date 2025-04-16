#ifndef ENGINE_APP_HPP
#define ENGINE_APP_HPP

#include "resources/ResourceManager.hpp"
#include "resources/types/TextFileResource.hpp"

namespace Engine {

class EngineApp {
public:
	static EngineApp* getInstance() { return s_instance; }

	EngineApp();

	void init();
	void run();
	void terminate();
	
	// Engine's user setup implementation.
	virtual void setup() = 0;

	void stop() { m_isRunning = false; }

private:
	static EngineApp* s_instance;

	ResourceManager<TextFileResource> m_textFilesResourceManager;

	bool m_isRunning;
};

} // Engine

#endif // !ENGINE_APP_HPP
