#include "physics/TilemapColliderGroup.hpp"

#include "core/pch.hpp"

namespace Engine {

std::vector<CollisionData> TilemapColliderGroup::query(const AABB& collider) const
{
	ASSERT(!m_bounds.isEmpty(), "TilemapColliderGroup bounds cannot be empty");

    std::vector<CollisionData> results;

    // Compute intersection with tilemap bounds
    AABB intersectionArea = AABB::intersectAABB(collider, m_bounds);
    if (intersectionArea.isEmpty()) return results;

    glm::vec2 tl = intersectionArea.Min;
    glm::vec2 br = intersectionArea.Max;

    glm::vec2 tlNormalised = (tl - m_bounds.Min) / tileSize;
    glm::vec2 brNormalised = (br - m_bounds.Min) / tileSize;

    // Compute start indices
    uint32_t startX = static_cast<uint32_t>(std::max(0.0f, std::floor(tlNormalised.x)));
    uint32_t startY = static_cast<uint32_t>(std::max(0.0f, std::floor(tlNormalised.y)));

    // Compute end indices
	// NOTE: Using epsilon avoids adding tiles at edges ([start, end] exclusive)
    float epsilon = 1e-4f;
    uint32_t endX = static_cast<uint32_t>(std::max(0.0f, std::floor(brNormalised.x - epsilon)));
    uint32_t endY = static_cast<uint32_t>(std::max(0.0f, std::floor(brNormalised.y - epsilon)));

    results.reserve((endX - startX + 1) * (endY - startY + 1));
    for (uint32_t y = startY; y <= endY; y++)
    {
        for (uint32_t x = startX; x <= endX; x++)
        {
            uint32_t index = x + y * static_cast<uint32_t>(tilemapSize.x);
            if (!m_tiles.isSet(index)) continue;

            glm::vec2 tileMin = m_bounds.Min + glm::vec2(x, y) * tileSize;
            glm::vec2 tileMax = tileMin + tileSize;
            results.emplace_back(CollisionData{ m_id, AABB(tileMin, tileMax) });
        }
    }

    return results;
}


} // Engine