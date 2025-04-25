#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <memory>
#include <queue>
#include <unordered_set>
#include <cstdint>
#include <thread>
#include <atomic>
#include <array>
#include <vector>
#include "block_data.h"
#include "chunk.h"
#include "utils/glm_hash.h"
#include "world/chunk_queue_node.h"
#include "utils/blocking_queue.h"
#include "utils/blocking_deque.h"
#include "world/chunk_snapshot.h"

class ChunkMap
{
public:
    ChunkMap();
    ~ChunkMap() = default;

    void update();

    void startBuildThread();
    void chunkBuildThreadFunc();
    void chunkLightThreadFunc();
    void stopThread() { m_stopThread = true; }

    void queueChunk(const glm::ivec3& chunkPos);
    void queueChunkRadius(const glm::ivec3& chunkPos, int radius);

    void setBlock(int x, int y, int z, BlockType type);
    void setBlock(const glm::ivec3& pos, BlockType type);

    void setBlockLight(int x, int y, int z, uint8_t lightLevel);
    void setBlockLight(const glm::ivec3& pos, uint8_t lightLevel);
    void setSunLight(int x, int y, int z, uint8_t lightLevel);
    void setSunLight(const glm::ivec3& pos, uint8_t lightLevel);

    void updateLighting(const glm::ivec3& chunkPos);
    void fillSunLight(ChunkSnapshotM& snapshot);

    BlockType getBlock(int x, int y, int z) const;
    BlockType getBlock(const glm::ivec3& pos) const;

    uint16_t getSunLight(int x, int y, int z) const;
    uint16_t getSunLight(const glm::ivec3& pos) const;
    uint16_t getBlockLight(int x, int y, int z) const;
    uint16_t getBlockLight(const glm::ivec3& pos) const;

    uint16_t getLightLevel(int x, int y, int z) const;
    uint16_t getLightLevel(const glm::ivec3& pos) const;

    std::shared_ptr<const Chunk> getChunk(int x, int y, int z) const;
    std::shared_ptr<const Chunk> getChunk(const glm::ivec3& pos) const;

    std::vector<std::shared_ptr<const Chunk>> getChunksInRadius(const glm::ivec3& chunkPos, int radius) const;
private:
    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>, glm_ivec3_hash, glm_ivec3_equal> m_chunks;
    BlockingDeque<std::shared_ptr<Chunk>> m_chunksToBuild;
    BlockingDeque<ChunkSnapshotM> m_chunksToFillLight;
    std::atomic_bool m_stopThread = false;

    std::shared_ptr<Chunk> getChunkInternal(const glm::ivec3& pos) const;
    std::shared_ptr<Chunk> checkCopy2Write(const std::shared_ptr<Chunk>& chunk);

    void floodFillLightAt(ChunkSnapshotM& snapshot, const glm::ivec3& pos, uint16_t value, bool isBlockLight = true);
    std::optional<ChunkSnapshotM> createSnapshotM(const glm::ivec3& centerChunkPos, std::vector<glm::ivec3>* missingChunks=nullptr, ChunkGenerationState minState = ChunkGenerationState::Complete);

    struct LightQueueNode
    {
        glm::ivec3 pos;
        uint16_t value;
    };
};