#include "physics/CollisionUtilities.hpp"
#include "utilities/Logger.hpp"

namespace TileBite {
namespace CollisionUtilities {

bool contains(const AABB& a, const OBB& b)
{
	for (auto c : b.getCorners())
	{
		if (!a.contains(c)) return false;
	}
	return true;
}

bool contains(const OBB& a, const AABB& b) {
	float cosR = cos(-a.Rotation);
	float sinR = sin(-a.Rotation);

	AABB obbToABB(-a.Size * 0.5f, a.Size * 0.5f);
	for (auto c : b.getCorners())
	{
		c -= a.Center;
		glm::vec2 rotated(
			c.x * cosR - c.y * sinR,
			c.x * sinR + c.y * cosR
		);

		if (!obbToABB.contains(rotated)) return false;
	}
	return true;
}

bool contains(const Collider& a, const OBB& b)
{
	return a.contains(b);
}

bool contains(const Collider& a, const AABB& b)
{
	return a.contains(b);
}

bool contains(const OBB& a, const Collider& b) {
	switch (b.Type) {
	case Collider::ColliderType::AABBType: return a.contains(b.AABBCollider);
	case Collider::ColliderType::OBBType:  return a.contains(b.OBBCollider);
	default: return false;
	}
}

bool contains(const AABB& a, const Collider& b) {
	switch (b.Type) {
	case Collider::ColliderType::AABBType: return a.contains(b.AABBCollider);
	case Collider::ColliderType::OBBType:  return a.contains(b.OBBCollider);
	default: return false;
	}
}

// =================================================

bool intersects(const AABB& a, const OBB& b)
{
	LOG_WARNING("Not implemented intersection AABB OBB");
	// TODO:
	return false;
}

bool intersects(const OBB& a, const AABB& b) {
	return intersects(b, a); // reuse symetrical logic
}

bool intersects(const Collider& a, const OBB& b)
{
	return a.intersects(b);
}

bool intersects(const Collider& a, const AABB& b)
{
	return a.intersects(b);
}

bool intersects(const OBB& a, const Collider& b) {
	return intersects(b, a); // reuse symetrical logic
}

bool intersects(const AABB& a, const Collider& b) {
	return intersects(b, a); // reuse symetrical logic
}

} // CollisionUtilities
} // TileBite