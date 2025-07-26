#ifndef AABB_TREE_HPP
#define AABB_TREE_HPP

#include "physics/AABB.hpp"

namespace Engine {

struct ColliderInfo {
	ID id;
	AABB collider;
};

// https://box2d.org/files/ErinCatto_DynamicBVH_GDC2019.pdf
class AABBTree {
public:
	void Insert(ID id, const ColliderInfo& collider);
	void Remove(ID id);
	void Update(ID id, const ColliderInfo& collider);
	std::vector<ID> Query(const AABB& collider) const;
private:
	struct Node {
		Node* Parent;
		std::unique_ptr<Node> Left;
		std::unique_ptr<Node> Right;

		AABB Bounds;
		std::optional<ColliderInfo> Value;

		bool IsLeaf() const { return !Left && !Right; }
	};

	Node* m_root;
};

} // Engine

#endif // !AABB_TREE_HPP