#ifndef HIERARCHIES_UPDATE_SYSTEM
#define HIERARCHIES_UPDATE_SYSTEM

#include "ecs/ISystem.hpp"
#include "ecs/types/EngineComponents.hpp"

namespace TileBite {

class HierarchiesUpdateSystem : public ISystem {
public:
	virtual void update(float deltaTime) override
	{
		auto& activeWorld = getSceneManager().getActiveScene()->getWorld();
		auto& activeSceneGraph = getSceneManager().getActiveScene()->getSceneGraph();

		// Adjust graph if connections changed
		activeWorld.query<ParentComponent>().each([&](ID entityID, ParentComponent* currentLink)
		{
			if (currentLink->isDirty())
			{
				if (currentLink->getParentID() != INVALID_ID)
				{
					activeSceneGraph.attachToParent(currentLink->getParentID(), entityID);
				}
				else
				{
					activeSceneGraph.detachFromParent(entityID);
				}

				currentLink->resetDirty();
			}
		});

		// Update world transforms via scene graph
		activeSceneGraph.updateWorldTransforms(activeWorld);
	}

};

} // TileBite

#endif // !HIERARCHIES_UPDATE_SYSTEM
