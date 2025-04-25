#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <array>
#include <vector>
#include <algorithm>
#include <optional>
#include <glm/glm.hpp>
#include "world/block_data.h"
#include "world/chunk_generation_state.h"

class Chunk;
class ChunkMap;

class ChunkSnapshot
{
public:
    std::array<std::shared_ptr<const Chunk>, 27> chunks;

    ChunkSnapshot();

    static std::optional<ChunkSnapshot> CreateSnapshot(const ChunkMap& chunkMap, const glm::ivec3& centerChunkPos, ChunkGenerationState minState = ChunkGenerationState::Complete);
    static std::optional<ChunkSnapshot> CreateSnapshot(const ChunkMap& chunkMap, const glm::ivec3& centerChunkPos, std::vector<glm::ivec3>* missingChunks, ChunkGenerationState minState = ChunkGenerationState::Complete);

    std::shared_ptr<const Chunk> center() const;

    static std::vector<glm::ivec3> getRequiredChunkDirs();

    // returns the chunk at a given local position.
    // local position is relative to the origin of the center chunk.
    std::shared_ptr<const Chunk> getChunkFromLocalPos(const glm::ivec3& localPos) const;
    BlockType getBlockFromLocalPos(const glm::ivec3& localPos) const;
    uint16_t getSunLightFromLocalPos(const glm::ivec3& localPos) const;
    uint16_t getBlockLightFromLocalPos(const glm::ivec3& localPos) const;
    uint16_t getLightLevelFromLocalPos(const glm::ivec3& localPos) const;
    uint16_t getNearbySkyLight(const glm::ivec3& localPos) const;
    uint16_t getNearbyBlockLight(const glm::ivec3& localPos) const;

    bool isValid(ChunkGenerationState minState = ChunkGenerationState::Complete) const;

    static glm::ivec3 getRelChunkPosFromLocalPos(const glm::ivec3& localPos);
    static bool inCenterBounds(const glm::ivec3& localPos);
};

// A modifiable version of ChunkSnapshot (holds shared pointers to non const Chunks)
class ChunkSnapshotM
{
public:
    std::array<std::shared_ptr<Chunk>, 27> chunks;

    ChunkSnapshotM() = default;

    ChunkSnapshotM(std::array<std::shared_ptr<Chunk>, 27> chunks) : chunks(chunks) {}

    std::shared_ptr<Chunk> getChunkFromLocalPos(const glm::ivec3& localPos);
    std::shared_ptr<const Chunk> getChunkFromLocalPos(const glm::ivec3& localPos) const;
    BlockType getBlockFromLocalPos(const glm::ivec3& localPos) const;
    uint16_t getSunLightFromLocalPos(const glm::ivec3& localPos) const;
    uint16_t getBlockLightFromLocalPos(const glm::ivec3& localPos) const;
    uint16_t getLightLevelFromLocalPos(const glm::ivec3& localPos) const;
    uint16_t getNearbySkyLight(const glm::ivec3& localPos) const;
    uint16_t getNearbyBlockLight(const glm::ivec3& localPos) const;

    void setBlockFromLocalPos(const glm::ivec3& localPos, BlockType type);
    void setSunLightFromLocalPos(const glm::ivec3& localPos, uint8_t lightLevel);
    void setBlockLightFromLocalPos(const glm::ivec3& localPos, uint8_t lightLevel);

    std::shared_ptr<Chunk> center() const { return chunks[13]; }
};