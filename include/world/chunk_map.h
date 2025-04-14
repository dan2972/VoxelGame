#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <memory>
#include <queue>
#include <cstdint>
#include "block_data.h"
#include "chunk.h"
#include "utils/glm_hash.h"
#include "world/chunk_queue_node.h"

class ChunkMap
{
public:
    ChunkMap();
    ~ChunkMap() = default;

    void update();

    void addChunkRadius(const glm::ivec3& chunkPos, int radius);

    bool addChunk(int x, int y, int z);
    bool addChunk(const glm::ivec3& position);

    void setBlock(int x, int y, int z, BlockType type);
    void setBlock(const glm::ivec3& pos, BlockType type);

    void setBlockLight(int x, int y, int z, uint8_t lightLevel);
    void setBlockLight(const glm::ivec3& pos, uint8_t lightLevel);
    void setSunLight(int x, int y, int z, uint8_t lightLevel);
    void setSunLight(const glm::ivec3& pos, uint8_t lightLevel);

    BlockType getBlock(int x, int y, int z) const;
    BlockType getBlock(const glm::ivec3& pos) const;

    uint16_t getSunLight(int x, int y, int z) const;
    uint16_t getSunLight(const glm::ivec3& pos) const;
    uint16_t getBlockLight(int x, int y, int z) const;
    uint16_t getBlockLight(const glm::ivec3& pos) const;

    uint16_t getLightLevel(int x, int y, int z) const;
    uint16_t getLightLevel(const glm::ivec3& pos) const;

    Chunk* getChunk(int x, int y, int z) const;
    Chunk* getChunk(const glm::ivec3& pos) const;

    std::vector<Chunk*> getChunksInRadius(const glm::ivec3& chunkPos, int radius) const;
private:
    std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>, glm_ivec3_hash, glm_ivec3_equal> m_chunks;
    std::priority_queue<ChunkQueueNode> m_chunkUpdateQueue;
};