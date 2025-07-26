#ifndef PHYSICS_ENGINE_HPP
#define PHYSICS_ENGINE_HPP

#include "utilities/Identifiable.hpp"
#include "physics/AABB.hpp"
#include "physics/AABBTree.hpp"
#include <vector>
#include <unordered_map>

namespace Engine {

class PhysicsEngine {
public:
	// Return all colliders overlapping with the given one
	std::vector<ID> queryCollisions(const AABB& collider) const;

	void addCollider(ID id, AABB* collider, TransformComponent* transform);
	void removeCollider(ID id);
	void updateCollider(ID id, AABB* collider, TransformComponent* transform);
	const std::vector<AABB> getAllColliders();
	const std::vector<AABB> getInternalBounds() const { return m_aabbTree.getInternalBounds(); }

private:
	AABBTree m_aabbTree;
};

} // Engine

#endif // PHYSICS_ENGINE_HPP
