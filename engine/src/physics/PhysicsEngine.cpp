#include "physics/PhysicsEngine.hpp"

namespace Engine {

std::vector<CollisionData> PhysicsEngine::queryCollisions(const AABB& collider) const
{
	std::vector<CollisionData> results;
	for (const auto& [id, aabb] : m_colliders) {
		if (collider.intersects(aabb))
			results.push_back({ id, aabb });
	}
	return results;
}

void PhysicsEngine::addCollider(ID id, AABB* collider, TransformComponent* transform)
{
	glm::vec2 min = collider->Min * transform->Size + transform->Position;
	glm::vec2 max = collider->Max * transform->Size + transform->Position;
	m_colliders[id] = AABB{ min, max };
}

void PhysicsEngine::removeCollider(ID id)
{
	m_colliders.erase(id);
}

void PhysicsEngine::updateCollider(ID id, AABB* aabb, TransformComponent* transform)
{
	auto it = m_colliders.find(id);
	if (it != m_colliders.end()) {
		glm::vec2 min = aabb->Min * transform->Size + transform->Position;
		glm::vec2 max = aabb->Max * transform->Size + transform->Position;
		it->second = AABB{ min, max };
	}
	else
	{
		// If the collider does not exist, add it
		addCollider(id, aabb, transform);
	}
}

void PhysicsEngine::clearColliders()
{
	m_colliders.clear();
}

} // Engine