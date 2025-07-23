#ifndef PHYSICS_ENGINE_HPP
#define PHYSICS_ENGINE_HPP

#include "utilities/Identifiable.hpp"
#include "physics/AABB.hpp"
#include <vector>

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

private:
	// TODO: update to AABBTree or similar for better performance
	std::vector<std::tuple<ID, AABB>> m_colliders;
};

} // Engine

#endif // PHYSICS_ENGINE_HPP
