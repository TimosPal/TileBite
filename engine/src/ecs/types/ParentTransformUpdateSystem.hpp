#ifndef PARENT_TRANSFORM_UPDATE_SYSTEM_HPP
#define PARENT_TRANSFORM_UPDATE_SYSTEM_HPP

#include "ecs/ISystem.hpp"
#include "ecs/types/EngineComponents.hpp"

namespace TileBite {

class ParentTransformUpdateSystem : public ISystem {
public:
	virtual void update(float deltaTime) override
	{
		auto& activeWorld = getSceneManager().getActiveScene()->getWorld();

		activeWorld.query<ParentLinkComponent>().each([&](ID entityID, ParentLinkComponent* currentLink)
		{
			currentLink->setCalculated(false);
		});

		activeWorld.query<ParentLinkComponent>().each([&](ID entityID, ParentLinkComponent* currentLink)
		{
			updateCachedTransform(activeWorld, entityID, currentLink);
		});
	}

	void updateCachedTransform(World& activeWorld, ID currentID, ParentLinkComponent* currentLink)
	{
		// TODO: handle missing transform components
		// TOOD: are more optimizations possible?

		if (currentLink == nullptr) return;
		if (currentLink->isCalculated()) return;
		currentLink->setCalculated(true);

		ParentLinkComponent* currentParent = nullptr;
		bool parentExists = activeWorld.entityExists(currentLink->ParentID);
		if (parentExists)
			currentParent = activeWorld.getComponent<ParentLinkComponent>(currentLink->ParentID);
		ID parentID = currentParent ? currentLink->ParentID : INVALID_ID;
		updateCachedTransform(activeWorld, parentID, currentParent);
		// Climb down hierarchy

		// Get parent's cached transform
		glm::vec2 parentPos = glm::vec2(0.0f);
		glm::vec2 parentSize = glm::vec2(1.0f);
		float parentRot = 0.0f;
		if (currentParent == nullptr)
		{
			if (parentExists)
			{
				TransformComponent* parentTransform = activeWorld.getComponent<TransformComponent>(currentLink->ParentID);
				parentPos = parentTransform->getPosition();
				parentSize = parentTransform->getSize();
				parentRot = parentTransform->getRotation();
			}
		}
		else
		{
			parentPos = currentParent->CachedParentTransform.getPosition();
			parentSize = currentParent->CachedParentTransform.getSize();
			parentRot = currentParent->CachedParentTransform.getRotation();
		}

		// Get local transform
		TransformComponent* currentTransform = activeWorld.getComponent<TransformComponent>(currentID);
		glm::vec2 localPos = currentTransform->getPosition();
		glm::vec2 localSize = currentTransform->getSize();
		float localRot = currentTransform->getRotation();

		// Apply parent's scale first, then rotation
		glm::vec2 scaledPos = localPos * parentSize;

		float s = sin(parentRot);
		float c = cos(parentRot);
		glm::vec2 rotatedPos;
		rotatedPos.x = scaledPos.x * c - scaledPos.y * s;
		rotatedPos.y = scaledPos.x * s + scaledPos.y * c;

		// Update cached transform
		currentLink->CachedParentTransform.setPosition(parentPos + rotatedPos);
		currentLink->CachedParentTransform.setSize(localSize * parentSize);
		currentLink->CachedParentTransform.setRotation(parentRot + localRot);

	}
};

} // TileBite

#endif // !PARENT_TRANSFORM_UPDATE_SYSTEM_HPP
