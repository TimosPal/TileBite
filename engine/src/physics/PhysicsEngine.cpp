#include "physics/PhysicsEngine.hpp"

namespace Engine {

std::vector<CollisionData> PhysicsEngine::queryCollisions(const AABB& collider) const
{
	std::vector<CollisionData> results;

	for (const auto& [id, aabb] : m_colliders) {
		if (collider.intersects(aabb))
		{
			results.push_back({ id, aabb });
		}
	}

	return results;
}

void PhysicsEngine::addCollider(ID id, AABB* collider, TransformComponent* transform)
{
	// Make world position AABB
	glm::vec2 min = collider->Min * transform->Size + transform->Position;
	glm::vec2 max = collider->Max * transform->Size + transform->Position;
	
	m_colliders.emplace_back(id, AABB{ min, max });
}

void PhysicsEngine::removeCollider(ID id)
{
	m_colliders.erase(
		std::remove_if(m_colliders.begin(), m_colliders.end(),
			[&](const std::tuple<ID, AABB>& entry) {
				return std::get<0>(entry) == id;
			}),
		m_colliders.end()
	);
}

void PhysicsEngine::updateCollider(ID id, AABB* collider, TransformComponent* transform)
{
	removeCollider(id);
	addCollider(id, collider, transform);
}

void PhysicsEngine::clearColliders()
{
	m_colliders.clear();
}

} // namespace Engine