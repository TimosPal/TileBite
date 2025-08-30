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

std::vector<CollisionData> TilemapColliderGroup::queryScanline(const OBB& collider) const
{
	// NOT USED CURRENTLY - but could be useful for performance testing / comparison

    std::vector<CollisionData> results;

    // Convert to tile coordinates
    auto tilePoints = collider.getCorners();
    for (auto& p : tilePoints) {
        p = (p - m_bounds.Min) / tileSize;
    }

    // Find min/max tile Y
    int minY = static_cast<int>(std::floor(std::min({ tilePoints[0].y, tilePoints[1].y, tilePoints[2].y, tilePoints[3].y })));
    int maxY = static_cast<int>(std::ceil(std::max({ tilePoints[0].y, tilePoints[1].y, tilePoints[2].y, tilePoints[3].y })));

    // Early reject if no vertical overlap
    if (maxY < 0 || minY >= tilemapSize.y)
        return results;

    // Clamp vertical range to tilemap
    minY = std::max(minY, 0);
    maxY = std::min(maxY, int(tilemapSize.y - 1));

    // Scanline loop
    for (int y = minY; y <= maxY; y++)
    {
        float scanY = y + 0.5f; // center of tile row

        float ix0, ix1;
        int intersectionCount = 0;

        // Find intersections with each edge
        for (int i = 0; i < 4; ++i)
        {
            const glm::vec2& p1 = tilePoints[i];
            const glm::vec2& p2 = tilePoints[(i + 1) % 4];

			// If the scanline is within the edges in y axis
            if ((scanY >= std::min(p1.y, p2.y)) && (scanY < std::max(p1.y, p2.y)))
            {
				// Intersection point based on linear interpolation
                // (solve for t where y = p1.y + t * (p2.y - p1.y))
                float t = (scanY - p1.y) / (p2.y - p1.y);
                float x = p1.x + t * (p2.x - p1.x);

                if (intersectionCount == 0) ix0 = x;
                else ix1 = x;

				// Skip further processing if we already have two intersections
                if (++intersectionCount == 2) break;
            }
        }

        if (intersectionCount == 2)
        {
            // Sort ao ix0 is left and ix1 is right
            if (ix0 > ix1) std::swap(ix0, ix1);

            int startX = static_cast<int>(std::floor(ix0));
            int endX = static_cast<int>(std::ceil(ix1));

            // Clamp horizontal range to tilemap
            startX = std::max(startX, 0);
            endX = std::min(endX, int(tilemapSize.x));

            // Shrinkg start and end X inwards by checking with SAT to prevent inaccurate false positives / true negatives
            // If we find 2 SAT tiles that are true then all the inbetween span must also be true.

            int startXFixed = startX;
            while (startXFixed < endX)
            {
                if (m_tiles.isSet(startXFixed + y * static_cast<uint32_t>(tilemapSize.x)))
                {
                    glm::vec2 tileMin = m_bounds.Min + glm::vec2(startXFixed, y) * tileSize;
                    glm::vec2 tileMax = tileMin + tileSize;
                    AABB tileCollider(tileMin, tileMax);
                    bool intersects = collider.intersects(tileCollider);
                    if (intersects) break;
                }

                startXFixed++;
            }

            int endXFixed = endX - 1;
            while (endXFixed >= startXFixed)
            {
                if (m_tiles.isSet(endXFixed + y * static_cast<uint32_t>(tilemapSize.x)))
                {
                    glm::vec2 tileMin = m_bounds.Min + glm::vec2(endXFixed, y) * tileSize;
                    glm::vec2 tileMax = tileMin + tileSize;
                    AABB tileCollider(tileMin, tileMax);
                    bool intersects = collider.intersects(tileCollider);
                    if (intersects) break;
                }

                endXFixed--;
            }

            for (int x = startXFixed; x <= endXFixed; x++)
            {
                if (m_tiles.isSet(x + y * static_cast<uint32_t>(tilemapSize.x))) {
                    glm::vec2 tileMin = m_bounds.Min + glm::vec2(x, y) * tileSize;
                    glm::vec2 tileMax = tileMin + tileSize;

                    AABB tileCollider(tileMin, tileMax);
                    bool intersects = collider.intersects(tileCollider);
                    if (!intersects)
                    {
                        continue;
                    }

                    results.emplace_back(CollisionData(TilemapCollisionData(
                        m_id, AABB(tileMin, tileMax), x, y
                    )));
                }
            }
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