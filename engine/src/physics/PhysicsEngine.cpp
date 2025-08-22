#include "physics/PhysicsEngine.hpp"
#include "utilities/assertions.hpp"

namespace Engine {

std::vector<CollisionData> PhysicsEngine::queryCollisions(const AABB& collider, ID excludeID) const
{
	// TODO: maybe use a different tree for static colliders
	// tilemaps could be simpified to 

	// Need to exclude the ID to avoid self-collision
	auto collisionData = m_coreTree.query(collider, excludeID);
	auto tilemapChunksCollisionData = m_tilemapColliderTree.query(collider, excludeID);

	collisionData.reserve(collisionData.size() + tilemapChunksCollisionData.size());
	for (const CollisionData& tilemapCollisionData : tilemapChunksCollisionData)
	{
		auto it = m_tilemapColliderGroups.find(tilemapCollisionData.Generic.id);
		ASSERT(it != m_tilemapColliderGroups.end(), "Tilemap collider group not found in the map");

		const TilemapColliderGroup& group = it->second;
		auto groupColliderData = group.query(collider);
		collisionData.insert(collisionData.end(), groupColliderData.begin(), groupColliderData.end());
	}

	return collisionData;
}

std::vector<RayHitData> PhysicsEngine::raycastAll(const Ray2D& ray) const
{

	auto rayHits = m_coreTree.raycastAll(ray);
	auto tilemapHits = m_tilemapColliderTree.raycastAll(ray);

	rayHits.reserve(rayHits.size() + tilemapHits.size());
	for (const RayHitData& tilemapHit : tilemapHits)
	{
		auto it = m_tilemapColliderGroups.find(tilemapHit.Generic.id);
		ASSERT(it != m_tilemapColliderGroups.end(), "Tilemap collider group not found in the map");
		const TilemapColliderGroup& group = it->second;
		auto groupRayHits = group.raycastAll(ray);
		rayHits.insert(rayHits.end(), groupRayHits.begin(), groupRayHits.end());
	}

	return rayHits;
}

std::optional<RayHitData> PhysicsEngine::raycastClosest(const Ray2D& ray) const
{
	auto rayHit = m_coreTree.raycastClosest(ray);

	// For tilemaps we iterate all intersectins chunks with the ray and find the cloest one that has
	// a intersected internal tile. This can be improved if we somehow use raycastClosest more efficiently.
	std::optional<RayHitData> closestTileHit;
	float tmin = std::numeric_limits<float>::max();
	auto tilemapHits = m_tilemapColliderTree.raycastAll(ray);

	for (const auto& hit : tilemapHits)
	{
		auto it = m_tilemapColliderGroups.find(hit.Generic.id);
		ASSERT(it != m_tilemapColliderGroups.end(), "Tilemap collider group not found in the map");
		const TilemapColliderGroup& group = it->second;

		auto groupRayHit = group.raycastClosest(ray);
		if (!groupRayHit.has_value()) continue;

		if (groupRayHit->tmin < tmin)
		{
			closestTileHit = groupRayHit;
			tmin = groupRayHit->tmin;
		}
	}

	if (rayHit.has_value() && closestTileHit.has_value())
		return (rayHit->tmin < closestTileHit->tmin) ? rayHit : closestTileHit;
	else
		return rayHit.has_value() ? rayHit : closestTileHit;
}

void PhysicsEngine::addCollider(ID id, AABB* collider, TransformComponent* transform)
{
	glm::vec2 min = collider->Min * transform->Size + transform->Position;
	glm::vec2 max = collider->Max * transform->Size + transform->Position;
	ColliderInfo info(id, AABB{min, max});
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
	ColliderInfo info(id, AABB{min, max});
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
	ColliderInfo info(id, bounds);
	m_tilemapColliderTree.insert(info);
}

void PhysicsEngine::updateTilemapColliderGroup(ID id, TransformComponent* transform, glm::vec2 tilemapSize, glm::vec2 tileSize, Bitset solidTiles)
{
	glm::vec2 min = transform->Position;
	glm::vec2 max = glm::vec2(tilemapSize.x * tileSize.x, tilemapSize.y * tileSize.y) * transform->Size + transform->Position;
	auto bounds = AABB(min, max);
	ColliderInfo info(id, bounds);
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