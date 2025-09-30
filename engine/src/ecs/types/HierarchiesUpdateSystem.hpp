#ifndef HIERARCHIES_UPDATE_SYSTEM
#define HIERARCHIES_UPDATE_SYSTEM

#include "ecs/ISystem.hpp"
#include "ecs/types/EngineComponents.hpp"

#include "core/EngineApp.hpp"

namespace TileBite {

class HierarchiesUpdateSystem : public ISystem {
public:
	virtual void update(float deltaTime) override
	{
        auto activeScene = EngineApp::getInstance()->getSceneManager().getActiveScene();
		auto& activeWorld = activeScene->getWorld();
        auto& activeSceneGraph = activeScene->getSceneGraph();

		// Adjust graph if connections changed
		activeWorld.query<ParentComponent>().each([&](ID entityID, ParentComponent* currentLink)
		{
			if (!currentLink->isDirty()) return;

			ID parentID = currentLink->getParentID();

			activeSceneGraph.attachToParent(parentID, entityID);
			TransformComponent* tr = activeWorld.getComponent<TransformComponent>(entityID);
			ASSERT(tr, "Linking to non transform components not allowed");

			// Convert to local space of new parent
			TransformComponent& parentWorldTr = activeSceneGraph.getWorldTransform(parentID);
			*tr = compose(inverse(parentWorldTr), *tr);

			currentLink->resetDirty();
		});

		// Update world transforms via scene graph
		activeSceneGraph.updateWorldTransforms();
	}

};

} // TileBite

#endif // !HIERARCHIES_UPDATE_SYSTEM
