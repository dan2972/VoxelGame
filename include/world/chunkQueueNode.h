#pragma once

#include <glm/glm.hpp>

struct ChunkQueueNode
{
    glm::ivec3 chunkPos;
    int distance;
    bool operator<(const ChunkQueueNode& other) const
    {
        return distance > other.distance;
    }
};