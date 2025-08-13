#include "physics/PhysicsEngine.hpp"
#include "utilities/assertions.hpp"

namespace Engine {

std::vector<CollisionData> PhysicsEngine::queryCollisions(const AABB& collider, ID excludeID) const
{
	// Need to exclude the ID to avoid self-collision
	auto collisionData = m_coreTree.query(collider, excludeID);
	auto tilemapChunksCollisionData = m_tilemapColliderTree.query(collider, excludeID);

	collisionData.reserve(collisionData.size() + tilemapChunksCollisionData.size());
	for (const CollisionData& tilemapCollisionData : tilemapChunksCollisionData)
	{
		auto it = m_tilemapColliderGroups.find(tilemapCollisionData.id);
		ASSERT(it != m_tilemapColliderGroups.end(), "Tilemap collider group not found in the map");

		const TilemapColliderGroup& group = it->second;
		auto groupColliderData = group.query(collider);
		collisionData.insert(collisionData.end(), groupColliderData.begin(), groupColliderData.end());
	}

	return collisionData;
}

void PhysicsEngine::addCollider(ID id, AABB* collider, TransformComponent* transform)
{
	glm::vec2 min = collider->Min * transform->Size + transform->Position;
	glm::vec2 max = collider->Max * transform->Size + transform->Position;
	ColliderInfo info{ id, AABB{min, max} };
	m_coreTree.insert(info);
}

void PhysicsEngine::removeCollider(ID id)
{
	m_coreTree.remove(id);
}

void PhysicsEngine::updateCollider(ID id, AABB* collider, TransformComponent* transform)
{
	glm::vec2 min = collider->Min * transform->Size + transform->Position;
	glm::vec2 max = collider->Max * transform->Size + transform->Position;
	ColliderInfo info{ id, AABB{min, max} };
	bool updated = m_coreTree.update(info);
	if (!updated) m_coreTree.insert(info);
}

void PhysicsEngine::addTilemapColliderGroup(ID id, TransformComponent* transform, glm::vec2 tilemapSize, glm::vec2 tileSize, Bitset solidTiles)
{
	glm::vec2 min = transform->Position;
	glm::vec2 max = glm::vec2(tilemapSize.x * tileSize.x, tilemapSize.y * tileSize.y) * transform->Size + transform->Position;
	auto bounds = AABB(min, max);
	m_tilemapColliderGroups.emplace(id, TilemapColliderGroup(id, bounds, tilemapSize, tileSize, solidTiles));
	
	// Create a collider for the tilemap group
	ColliderInfo info{ id, bounds };
	m_tilemapColliderTree.insert(info);
}

void PhysicsEngine::updateTilemapColliderGroup(ID id, TransformComponent* transform, glm::vec2 tilemapSize, glm::vec2 tileSize, Bitset solidTiles)
{
	glm::vec2 min = transform->Position;
	glm::vec2 max = glm::vec2(tilemapSize.x * tileSize.x, tilemapSize.y * tileSize.y) * transform->Size + transform->Position;
	auto bounds = AABB(min, max);
	ColliderInfo info{ id, bounds };
	bool updated = m_tilemapColliderTree.update(info);
	if (!updated)
	{
		m_tilemapColliderTree.insert(info);
		m_tilemapColliderGroups.emplace(id, TilemapColliderGroup(id, bounds, tilemapSize, tileSize, solidTiles));
	}
	else
	{
		m_tilemapColliderGroups[id] = TilemapColliderGroup(id, bounds, tilemapSize, tileSize, solidTiles);
	}
}

} // Engine