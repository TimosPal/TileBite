#include "physics/PhysicsEngine.hpp"

namespace Engine {

std::vector<ID> PhysicsEngine::queryCollisions(const AABB& collider) const
{
	return m_aabbTree.query(collider);
}

void PhysicsEngine::addCollider(ID id, AABB* collider, TransformComponent* transform)
{
	glm::vec2 min = collider->Min * transform->Size + transform->Position;
	glm::vec2 max = collider->Max * transform->Size + transform->Position;
	ColliderInfo info{ id, AABB{min, max} };
	m_aabbTree.insert(info);
}

void PhysicsEngine::removeCollider(ID id)
{
	m_aabbTree.remove(id);
}

void PhysicsEngine::updateCollider(ID id, AABB* collider, TransformComponent* transform)
{
	glm::vec2 min = collider->Min * transform->Size + transform->Position;
	glm::vec2 max = collider->Max * transform->Size + transform->Position;
	ColliderInfo info{ id, AABB{min, max} };
	bool updated = m_aabbTree.update(info);
	if (!updated) m_aabbTree.insert(info);
}

const std::vector<AABB> PhysicsEngine::getAllColliders()
{
	return m_aabbTree.getLeafColliders();
}

} // Engine