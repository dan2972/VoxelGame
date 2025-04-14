#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <array>
#include <vector>
#include <algorithm>
#include "world/chunk.h"
#include "world/chunk_map.h"
#include "world/block_data.h"

class ChunkSnapshot
{
public:
    std::array<std::shared_ptr<const Chunk>, 27> chunks;

    ChunkSnapshot();
    ChunkSnapshot(const std::array<std::shared_ptr<const Chunk>, 27>& chunks);
    ChunkSnapshot(const ChunkMap& chunkMap, const glm::ivec3& centerChunkPos);

    static bool CreateSnapshot(const ChunkMap& chunkMap, const glm::ivec3& centerChunkPos, ChunkSnapshot& snapshot);

    std::shared_ptr<const Chunk> center() const;

    static std::vector<glm::ivec3> getRequiredChunkDirs();

    // returns the chunk at a given local position.
    // local position is relative to the origin of the center chunk.
    std::shared_ptr<const Chunk> getChunkFromLocalPos(const glm::ivec3& localPos) const;
    BlockType getBlockFromLocalPos(const glm::ivec3& localPos) const;
    uint16_t getSunLightFromLocalPos(const glm::ivec3& localPos) const;
    uint16_t getBlockLightFromLocalPos(const glm::ivec3& localPos) const;
    uint16_t getLightLevelFromLocalPos(const glm::ivec3& localPos) const;

    bool isValid() const;

private:
    static glm::ivec3 getRelChunkPosFromLocalPos(const glm::ivec3& localPos);
};