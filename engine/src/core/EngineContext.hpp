#ifndef ENGINE_CONTEXT_HPP
#define ENGINE_CONTEXT_HPP

#include "core/pch.hpp"
#include "events/Event.hpp"

namespace Engine {

// Forward declarations
class SceneManager;
class AssetsManager;
class EventDispatcher;

class IEngineContext {
public:
	virtual ~IEngineContext() = default;

	virtual SceneManager& getSceneManager() = 0;
	virtual AssetsManager& getAssetsManager() = 0;
	virtual EventDispatcher& getCoreEventDispatcher() = 0;

	virtual void pushEvent(std::unique_ptr<Event> event) = 0;
};

class InjectEngineContext : protected IEngineContext {
public:
	void setSceneManager(SceneManager* sceneManager);
	void setAssetsManager(AssetsManager* assets);
	void setCoreEventDispatcher(EventDispatcher* coreEventDispatcher);
	void setPushEventCallable(std::function<void(std::unique_ptr<Event>)> pushEventCallable);

protected:
	// TODO: make other interface for client side use.

	SceneManager& getSceneManager() override;
	AssetsManager& getAssetsManager() override;
	EventDispatcher& getCoreEventDispatcher() override;
	std::function<void(std::unique_ptr<Event>)> getPushEventCallable();
	
	void pushEvent(std::unique_ptr<Event> event) override;
private:
	SceneManager* m_sceneManager = nullptr;
	AssetsManager* m_assetsManager = nullptr;
	EventDispatcher* m_coreEventDispatcher = nullptr;

	std::function<void(std::unique_ptr<Event>)> m_pushEventCallable;

};

} // namespace Engine

#endif // ENGINE_CONTEXT_HPP
