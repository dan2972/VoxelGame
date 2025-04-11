#pragma once

#include <glm/glm.hpp>
#include <functional>

struct glm_ivec3_hash
{
    std::size_t operator()(const glm::ivec3& coord) const
    {
        std::hash<int> hasher;
        size_t hash = 0;
        hash ^= hasher(coord.x) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= hasher(coord.y) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= hasher(coord.z) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        return hash;
    }
};

struct glm_ivec3_equal
{
    bool operator()(const glm::ivec3& lhs, const glm::ivec3& rhs) const
    {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }
};