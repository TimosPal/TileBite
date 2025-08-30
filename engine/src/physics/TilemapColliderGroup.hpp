#ifndef TILEMAP_COLLIDER_GROUP_HPP
#define TILEMAP_COLLIDER_GROUP_HPP

#include "physics/AABB.hpp"
#include "physics/CollisionData.hpp"
#include "physics/Ray2D.hpp"
#include "ecs/types/EngineComponents.hpp"
#include "utilities/Bitset.hpp"

namespace TileBite {

class TilemapColliderGroup {
public:
	TilemapColliderGroup() = default;
	TilemapColliderGroup(ID tilemapID, const AABB& bounds, glm::vec2 tilemapSize, glm::vec2 tileSize, Bitset solidTiles)
		: m_bounds(bounds), m_tiles(solidTiles),
		tilemapSize(tilemapSize), tileSize(tileSize), m_id(tilemapID)
	{}

    // Generic scaning method within bounding box area with custom collision test for each tile
    // Implement overloaded function manualy if needing custom implementation
    template<typename ColliderT>
    std::vector<CollisionData> query(const ColliderT& collider) const
    {
        std::vector<CollisionData> results;

        // Search are is the collider's bound box in AABB space
        AABB worldAABB = collider.getBoundingBox();
        AABB clampedAABB = AABB::intersectionBound(m_bounds, worldAABB);
        clampedAABB.Min = worldPositionToTileIndices(clampedAABB.Min);
        clampedAABB.Max = worldPositionToTileIndices(clampedAABB.Max);
        // Clamped to be within tilemap bounds

        for (int y = clampedAABB.Min.y; y <= clampedAABB.Max.y; ++y)
        {
            for (int x = clampedAABB.Min.x; x <= clampedAABB.Max.x; ++x)
            {
                uint32_t idx = x + y * uint32_t(tilemapSize.x);
                if (!m_tiles.isSet(idx))
                    continue;

                glm::vec2 tileMin = m_bounds.Min + glm::vec2(x, y) * tileSize;
                glm::vec2 tileMax = tileMin + tileSize;
                AABB tileAABB(tileMin, tileMax);

                // SAT check against each tile AABB collider
                if (collider.intersects(tileAABB))
                {
                    results.emplace_back(
                        CollisionData(
                            TilemapCollisionData(m_id, tileAABB, x, y)
                        )
                    );
                }
            }
        }

        return results;
    }

	std::vector<CollisionData> query(const AABB& collider) const;
    std::vector<CollisionData> queryScanline(const OBB& collider) const;
	std::vector<RayHitData> raycastAll(const Ray2D& ray) const;
	std::optional<RayHitData> raycastClosest(const Ray2D& ray) const;
private:
	AABB m_bounds; // The bounding box of the tilemap collider group
	glm::vec2 tilemapSize, tileSize;
	
	Bitset m_tiles; // Bitset representing the tiles in the group
	ID m_id = INVALID_ID; // Unique ID for the tilemap collider group

	std::vector<RayHitData> ADDSearch(const Ray2D& ray, bool stopAtFirst) const;
    std::vector<glm::ivec2> ADDRasterization(glm::vec2 start, glm::vec2 end) const;

	glm::ivec2 worldPositionToTileIndices(glm::vec2 position, float epsilon = 0) const;
	
    template <typename Callback>
    void ADDWalker(glm::vec2 start, glm::vec2 end, Callback&& callback) const
    {
        glm::vec2 lineDir = glm::normalize(end - start);
        glm::vec2 lineDirLocal = lineDir / tileSize;
        glm::vec2 rayStartLocal = (start - m_bounds.Min) / tileSize;

        glm::ivec2 currentTile = floor(rayStartLocal);
        glm::ivec2 step;
        glm::vec2 sideDist;
        glm::vec2 deltaDist;

        // Step size (distance to next grid line in each axis)
        deltaDist.x = (lineDirLocal.x == 0.0f) ? std::numeric_limits<float>::infinity() : std::abs(1.0f / lineDirLocal.x);
        deltaDist.y = (lineDirLocal.y == 0.0f) ? std::numeric_limits<float>::infinity() : std::abs(1.0f / lineDirLocal.y);

        // Step direction and initial sideDist
        if (lineDirLocal.x < 0) {
            step.x = -1;
            sideDist.x = (rayStartLocal.x - currentTile.x) * deltaDist.x;
        }
        else {
            step.x = 1;
            sideDist.x = (currentTile.x + 1.0f - rayStartLocal.x) * deltaDist.x;
        }

        if (lineDirLocal.y < 0) {
            step.y = -1;
            sideDist.y = (rayStartLocal.y - currentTile.y) * deltaDist.y;
        }
        else {
            step.y = 1;
            sideDist.y = (currentTile.y + 1.0f - rayStartLocal.y) * deltaDist.y;
        }

        float tileDistance = 0.0f;
        float maxLength = glm::length(end - start);
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
                if (!callback(currentTile)) break;
            }
        }
    }
};

} // TileBite

#endif // TILEMAP_COLLIDER_GROUP_HPP