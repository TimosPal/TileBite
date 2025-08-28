#include "physics/TilemapColliderGroup.hpp"

#include "core/pch.hpp"

namespace TileBite {

glm::vec2 TilemapColliderGroup::worldPositionToTileIndices(glm::vec2 position, float epsilon) const
{
    // NOTE: Using epsilon avoids adding tiles at edges ([start, end] exclusive)
    glm::vec2 normalisedPosition = (position - m_bounds.Min) / tileSize;
    uint32_t xIndex = static_cast<uint32_t>(std::max(0.0f, std::floor(normalisedPosition.x + epsilon)));
    uint32_t yIndex = static_cast<uint32_t>(std::max(0.0f, std::floor(normalisedPosition.y + epsilon)));

    return { xIndex, yIndex };
}

std::vector<CollisionData> TilemapColliderGroup::query(const OBB& collider) const
{
    // TODO:
    NOT_IMPLEMENTED_LOG;
    return {};
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

    glm::vec2 rayDir = ray.getDirection();
    glm::vec2 rayDirLocal = rayDir / tileSize;
	float startingT = std::max(0.0f, tmin); // If tmin is negative we are inside the tilemap so we start at 0
    glm::vec2 rayStart = ray.at(startingT - 0.01f); // Slightly offset to avoid missing first tile
    glm::vec2 rayStartLocal = (rayStart - m_bounds.Min) / tileSize;
    glm::vec2 rayEnd = ray.at(ray.getMaxT());
    glm::vec2 rayEndLocal = (rayEnd - m_bounds.Min) / tileSize;

    glm::ivec2 currentTile = glm::floor(rayStartLocal);
    glm::ivec2 step;
    glm::vec2 sideDist;
    glm::vec2 deltaDist;

    // Step size (distance to next grid line in each axis)
    deltaDist.x = (rayDirLocal.x == 0.0f) ? std::numeric_limits<float>::infinity() : std::abs(1.0f / rayDirLocal.x);
    deltaDist.y = (rayDirLocal.y == 0.0f) ? std::numeric_limits<float>::infinity() : std::abs(1.0f / rayDirLocal.y);

    // Step direction and initial sideDist
    if (rayDirLocal.x < 0) {
        step.x = -1;
        sideDist.x = (rayStartLocal.x - currentTile.x) * deltaDist.x;
    }
    else {
        step.x = 1;
        sideDist.x = (currentTile.x + 1.0f - rayStartLocal.x) * deltaDist.x;
    }

    if (rayDirLocal.y < 0) {
        step.y = -1;
        sideDist.y = (rayStartLocal.y - currentTile.y) * deltaDist.y;
    }
    else {
        step.y = 1;
        sideDist.y = (currentTile.y + 1.0f - rayStartLocal.y) * deltaDist.y;
    }

    float tileDistance = 0.0f;
    float maxLength = glm::length(rayEnd - rayStart);
    while (true) {
        if (sideDist.x < sideDist.y) {
            tileDistance = sideDist.x;
            if (tileDistance > maxLength) break; 
            sideDist.x += deltaDist.x;
            currentTile.x += step.x;
        }
        else {
            tileDistance = sideDist.y;
            if (tileDistance > maxLength) break;
            sideDist.y += deltaDist.y;
            currentTile.y += step.y;
        }

        if (currentTile.x >= 0 && currentTile.x < tilemapSize.x &&
            currentTile.y >= 0 && currentTile.y < tilemapSize.y)
        {
            if (m_tiles.isSet(currentTile.x + currentTile.y * tilemapSize.x)) {
                AABB tileBounds(
                    m_bounds.Min + glm::vec2(currentTile) * tileSize,
                    m_bounds.Min + glm::vec2(currentTile + glm::ivec2(1)) * tileSize
                );
                float tileTmin, tileTmax;
                ray.intersect(tileBounds, tileTmin, tileTmax);
                results.push_back(RayHitData(
                    TilemapCollisionData(m_id, tileBounds, currentTile.x, currentTile.y),
                    tileTmin, tileTmax)
                );

                if (stopAtFirst) break;
            }
        }
    }

    return results;
}

} // TileBite