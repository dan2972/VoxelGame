#pragma once

#include <glm/glm.hpp>
#include <queue>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include "block_data.h"
#include "chunk.h"
#include "utils/glm_hash.h"
#include "world/chunk_queue_node.h"
#include "world/chunk_map.h"

class World
{
public:
    World();
    ~World() = default;

    ChunkMap& getChunkMap() { return m_chunkMap; }

    void update();
private:
    ChunkMap m_chunkMap;
};