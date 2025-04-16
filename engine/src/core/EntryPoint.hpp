#include <memory>

#include "core/EngineApp.hpp"

// Call this in your EngineApp to declare the extern function.
#define CREATE_ENGINE_APPLICATION(EngineAppClass) \
    std::unique_ptr<Engine::EngineApp> createEngineApp() { return std::make_unique<EngineAppClass>(); }

extern std::unique_ptr<Engine::EngineApp> createEngineApp();

int main()
{
	auto app = createEngineApp();
	app->init();
	app->setup();
	app->run();
	app->terminate();

	return 0;
}