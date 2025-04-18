#pragma once

#include <vector>
#include <functional>
#include <algorithm>
#include <glm/glm.hpp>

namespace algo
{
    struct VoxelRayHitNode
    {
        glm::ivec3 pos;
        glm::ivec3 normal;
    };

    VoxelRayHitNode voxelRayHit(
        const glm::vec3& origin, 
        const glm::vec3& dir, 
        std::function<bool (const glm::ivec3&)> hitCondition = [](const glm::ivec3&) { return false; }, 
        float maxDistance=100.0f);

    std::vector<VoxelRayHitNode> voxelRayTraversal(
        const glm::vec3& origin, 
        const glm::vec3& dir, 
        std::function<bool (const glm::ivec3&)> hitCondition = [](const glm::ivec3&) { return false; }, 
        float maxDistance=100.0f);

    std::vector<glm::ivec3> getPosFromCenter(const glm::ivec3& center, int radius);
}