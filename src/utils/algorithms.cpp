#include "utils/algorithms.h"

namespace algo
{
    VoxelRayHitNode voxelRayHit(
        const glm::vec3& origin, 
        const glm::vec3& dir, 
        std::function<bool (const glm::ivec3&)> hitCondition, 
        float maxDistance)
    {
        glm::ivec3 v = glm::floor(origin);
        glm::ivec3 step = glm::sign(dir);
        float tmaxX = dir.x != 0.0f ? (v.x + (step.x > 0 ? 1 : 0) - origin.x) / dir.x : std::numeric_limits<float>::max();
        float tmaxY = dir.y != 0.0f ? (v.y + (step.y > 0 ? 1 : 0) - origin.y) / dir.y : std::numeric_limits<float>::max();
        float tmaxZ = dir.z != 0.0f ? (v.z + (step.z > 0 ? 1 : 0) - origin.z) / dir.z : std::numeric_limits<float>::max();

        float tDeltaX = step.x != 0.0f ? std::abs(1.0f / dir.x) : std::numeric_limits<float>::max();
        float tDeltaY = step.y != 0.0f ? std::abs(1.0f / dir.y) : std::numeric_limits<float>::max();
        float tDeltaZ = step.z != 0.0f ? std::abs(1.0f / dir.z) : std::numeric_limits<float>::max();

        glm::ivec3 hitNormal;
        float tTotal = std::min({tmaxX, tmaxY, tmaxZ});

        while (tTotal < maxDistance) {
            if (tmaxX < tmaxY && tmaxX < tmaxZ) {
                v.x += step.x;
                tmaxX += tDeltaX;
                hitNormal = glm::ivec3(-step.x, 0, 0);
            } else if (tmaxY < tmaxZ) {
                v.y += step.y;
                tmaxY += tDeltaY;
                hitNormal = glm::ivec3(0, -step.y, 0);
            } else {
                v.z += step.z;
                tmaxZ += tDeltaZ;
                hitNormal = glm::ivec3(0, 0, -step.z);
            }
            tTotal = std::min({tmaxX, tmaxY, tmaxZ});

            if (hitCondition(v)) {
                return {v, hitNormal};
            }
        }
        return {v, glm::ivec3(0)}; // No hit found
    }

    std::vector<VoxelRayHitNode> voxelRayTraversal(
        const glm::vec3& origin, 
        const glm::vec3& dir, 
        std::function<bool (const glm::ivec3&)> hitCondition, 
        float maxDistance)
    {
        std::vector<VoxelRayHitNode> hits;
        glm::ivec3 v = glm::floor(origin);
        glm::ivec3 step = glm::sign(dir);
        float tmaxX = dir.x != 0.0f ? (v.x + (step.x > 0 ? 1 : 0) - origin.x) / dir.x : std::numeric_limits<float>::max();
        float tmaxY = dir.y != 0.0f ? (v.y + (step.y > 0 ? 1 : 0) - origin.y) / dir.y : std::numeric_limits<float>::max();
        float tmaxZ = dir.z != 0.0f ? (v.z + (step.z > 0 ? 1 : 0) - origin.z) / dir.z : std::numeric_limits<float>::max();

        float tDeltaX = step.x != 0.0f ? std::abs(1.0f / dir.x) : std::numeric_limits<float>::max();
        float tDeltaY = step.y != 0.0f ? std::abs(1.0f / dir.y) : std::numeric_limits<float>::max();
        float tDeltaZ = step.z != 0.0f ? std::abs(1.0f / dir.z) : std::numeric_limits<float>::max();

        glm::ivec3 hitNormal;
        float tTotal = std::min({tmaxX, tmaxY, tmaxZ});
        hits.emplace_back(v, glm::ivec3(0)); // Initial voxel

        while (tTotal < maxDistance) {
            if (tmaxX < tmaxY && tmaxX < tmaxZ) {
                v.x += step.x;
                tmaxX += tDeltaX;
                hitNormal = glm::ivec3(-step.x, 0, 0);
                hits.emplace_back(v, hitNormal);
            } else if (tmaxY < tmaxZ) {
                v.y += step.y;
                tmaxY += tDeltaY;
                hitNormal = glm::ivec3(0, -step.y, 0);
                hits.emplace_back(v, hitNormal);
            } else {
                v.z += step.z;
                tmaxZ += tDeltaZ;
                hitNormal = glm::ivec3(0, 0, -step.z);
                hits.emplace_back(v, hitNormal);
            }
            tTotal = std::min({tmaxX, tmaxY, tmaxZ});

            if (hitCondition(v)) {
                return hits;
            }
        }
        return hits;
    }

    std::vector<glm::ivec3> getPosFromCenter(const glm::ivec3& center, int radius)
    {
        std::vector<glm::ivec3> positions;

        for (int x = -radius; x <= radius; ++x) {
            for (int y = -radius; y <= radius; ++y) {
                for (int z = -radius; z <= radius; ++z) {
                    glm::ivec3 offset(x, y, z);
                    positions.emplace_back(center + offset);
                }
            }
        }

        std::sort(positions.begin(), positions.end(), [center](const glm::ivec3& a, const glm::ivec3& b) {
            int distA = glm::abs(a.x - center.x) + glm::abs(a.y - center.y) + glm::abs(a.z - center.z);
            int distB = glm::abs(b.x - center.x) + glm::abs(b.y - center.y) + glm::abs(b.z - center.z);
            return distA < distB;
        });

        return positions;
    }
}