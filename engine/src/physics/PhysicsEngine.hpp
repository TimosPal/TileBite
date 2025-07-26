#ifndef PHYSICS_ENGINE_HPP
#define PHYSICS_ENGINE_HPP

#include "utilities/Identifiable.hpp"
#include "physics/AABB.hpp"
#include <vector>
#include <unordered_map>

namespace Engine {

struct CollisionData {
	ID id;
	AABB collider;

	// TODO: Collided area info
};

class PhysicsEngine {
public:
	// Return all colliders overlapping with the given one
	std::vector<CollisionData> queryCollisions(const AABB& collider) const;

	void addCollider(ID id, AABB* collider, TransformComponent* transform);
	void removeCollider(ID id);
	void updateCollider(ID id, AABB* collider, TransformComponent* transform);
	void clearColliders();
	const std::unordered_map<ID, AABB>& getAllColliders() { return m_colliders; }

private:
	// TODO: update to AABBTree or similar for better performance
	std::unordered_map<ID, AABB> m_colliders;
};

} // Engine

#endif // PHYSICS_ENGINE_HPP
