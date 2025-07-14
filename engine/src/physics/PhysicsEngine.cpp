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

void PhysicsEngine::addCollider(const AABB& collider, ID id)
{
	m_colliders.emplace_back(id, collider);
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

void PhysicsEngine::clearColliders()
{
	m_colliders.clear();
}

} // namespace Engine