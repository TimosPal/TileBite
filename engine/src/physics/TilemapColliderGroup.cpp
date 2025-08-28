#include "physics/TilemapColliderGroup.hpp"

#include "core/pch.hpp"

namespace TileBite {

glm::ivec2 TilemapColliderGroup::worldPositionToTileIndices(glm::vec2 position, float epsilon) const
{
    // NOTE: Using epsilon avoids adding tiles at edges ([start, end] exclusive)
    glm::vec2 normalisedPosition = (position - m_bounds.Min) / tileSize;
    uint32_t xIndex = static_cast<uint32_t>(std::max(0.0f, std::floor(normalisedPosition.x + epsilon)));
    uint32_t yIndex = static_cast<uint32_t>(std::max(0.0f, std::floor(normalisedPosition.y + epsilon)));

    return { xIndex, yIndex };
}

std::vector<CollisionData> TilemapColliderGroup::query(const OBB& collider) const
{
    std::vector<CollisionData> results;

    auto points = collider.getCorners();

    // TODO: placeholder code

    for (glm::ivec2& tile : ADDRasterization(points[0], points[1]))
    {
        if (m_tiles.isSet(tile.x + tile.y * static_cast<uint32_t>(tilemapSize.x))) {
            glm::vec2 tileMin = m_bounds.Min + glm::vec2(tile) * tileSize;
            glm::vec2 tileMax = tileMin + tileSize;
            results.emplace_back(CollisionData(TilemapCollisionData(m_id, AABB(tileMin, tileMax), tile.x, tile.y)));
		}
    }

    for (glm::ivec2& tile : ADDRasterization(points[1], points[2]))
    {
        if (m_tiles.isSet(tile.x + tile.y * static_cast<uint32_t>(tilemapSize.x))) {
            glm::vec2 tileMin = m_bounds.Min + glm::vec2(tile) * tileSize;
            glm::vec2 tileMax = tileMin + tileSize;
            results.emplace_back(CollisionData(TilemapCollisionData(m_id, AABB(tileMin, tileMax), tile.x, tile.y)));
        }
    }

    for (glm::ivec2& tile : ADDRasterization(points[2], points[3]))
    {
        if (m_tiles.isSet(tile.x + tile.y * static_cast<uint32_t>(tilemapSize.x))) {
            glm::vec2 tileMin = m_bounds.Min + glm::vec2(tile) * tileSize;
            glm::vec2 tileMax = tileMin + tileSize;
            results.emplace_back(CollisionData(TilemapCollisionData(m_id, AABB(tileMin, tileMax), tile.x, tile.y)));
        }
    }

    for (glm::ivec2& tile : ADDRasterization(points[3], points[0]))
    {
        if (m_tiles.isSet(tile.x + tile.y * static_cast<uint32_t>(tilemapSize.x))) {
            glm::vec2 tileMin = m_bounds.Min + glm::vec2(tile) * tileSize;
            glm::vec2 tileMax = tileMin + tileSize;
            results.emplace_back(CollisionData(TilemapCollisionData(m_id, AABB(tileMin, tileMax), tile.x, tile.y)));
        }
    }

    return results;
}

std::vector<CollisionData> TilemapColliderGroup::query(const AABB& collider) const
{
	ASSERT(!m_bounds.isEmpty(), "TilemapColliderGroup bounds cannot be empty");

    std::vector<CollisionData> results;

    // Compute intersection with tilemap bounds
    AABB intersectionArea = AABB::intersectionBound(collider, m_bounds);
    if (intersectionArea.isEmpty()) return results;

	glm::vec2 startIndices = worldPositionToTileIndices(intersectionArea.Min);
	glm::vec2 endIndices = worldPositionToTileIndices(intersectionArea.Max, -1e-4f);

	// TODO: this for loop can be optimized by using bitset operations
    results.reserve((endIndices.x - startIndices.x + 1) * (endIndices.y - startIndices.y + 1));
    for (uint32_t y = startIndices.y; y <= endIndices.y; y++)
    {
        for (uint32_t x = startIndices.x; x <= endIndices.x; x++)
        {
            uint32_t index = x + y * static_cast<uint32_t>(tilemapSize.x);
            if (!m_tiles.isSet(index)) continue;

            glm::vec2 tileMin = m_bounds.Min + glm::vec2(x, y) * tileSize;
            glm::vec2 tileMax = tileMin + tileSize;
            results.emplace_back(CollisionData(TilemapCollisionData(m_id, AABB(tileMin, tileMax), x, y)));
        }
    }

    return results;
}

std::vector<RayHitData> TilemapColliderGroup::raycastAll(const Ray2D& ray) const {
    return ADDSearch(ray, false);
}

std::optional<RayHitData> TilemapColliderGroup::raycastClosest(const Ray2D& ray) const {
    auto result = ADDSearch(ray, true);
    return result.size() > 0 ? std::optional(result.back()) : std::nullopt;
}

std::vector<RayHitData> TilemapColliderGroup::ADDSearch(const Ray2D& ray, bool stopAtFirst) const {
    std::vector<RayHitData> results;
    
    float tmin, tmax;
    if (!ray.intersect(m_bounds, tmin, tmax))
        return results;

	float startingT = std::max(0.0f, tmin); // If tmin is negative we are inside the tilemap so we start at 0
    glm::vec2 rayStart = ray.at(startingT - 0.01f); // Slightly offset to avoid missing first tile
    glm::vec2 rayEnd = ray.at(ray.getMaxT());

    ADDWalker(rayStart, rayEnd, [&](const glm::ivec2& tile) {
        if (m_tiles.isSet(tile.x + tile.y * tilemapSize.x)) {
            AABB tileBounds(
                m_bounds.Min + glm::vec2(tile) * tileSize,
                m_bounds.Min + glm::vec2(tile + glm::ivec2(1)) * tileSize
            );
            float tileTmin, tileTmax;
            ray.intersect(tileBounds, tileTmin, tileTmax);
            results.push_back(RayHitData(
                TilemapCollisionData(m_id, tileBounds, tile.x, tile.y),
                tileTmin, tileTmax)
            );

            if (stopAtFirst) return false;
        }

        return true;
    });

    return results;
}

std::vector<glm::ivec2> TilemapColliderGroup::ADDRasterization(glm::vec2 start, glm::vec2 end) const {
	// Assume a ray (line segment) from start to end and return all the tile indices it intersects

    std::vector<glm::ivec2> results;

	glm::vec2 rayDir = end - start;
	Ray2D ray(start, rayDir, glm::length(rayDir));

    float tmin, tmax;
    if (!ray.intersect(m_bounds, tmin, tmax))
        return results;

    float startingT = std::max(0.0f, tmin); // If tmin is negative we are inside the tilemap so we start at 0
    glm::vec2 rayStart = ray.at(startingT - 0.01f); // Slightly offset to avoid missing first tile
    glm::vec2 rayEnd = ray.at(ray.getMaxT());

    ADDWalker(start, end, [&](const glm::ivec2& tile) {
        results.push_back(tile);
		return true;
    });

    return results;
}

} // TileBite