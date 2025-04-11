#pragma once

#include <glm/glm.hpp>
#include <array>
#include "block_data.h"

class Chunk
{
public:
    const static int CHUNK_SIZE = 16;

    Chunk();
    Chunk(const glm::ivec3& position);
    ~Chunk() = default;

    void generateTerrain();

    BlockType getBlock(int x, int y, int z) const;
    BlockType getBlock(const glm::ivec3& pos) const;

    uint16_t getSunLight(int x, int y, int z) const;
    uint16_t getSunLight(const glm::ivec3& pos) const;
    uint16_t getBlockLight(int x, int y, int z) const;
    uint16_t getBlockLight(const glm::ivec3& pos) const;

    void setBlock(int x, int y, int z, BlockType type);
    void setBlock(const glm::ivec3& pos, BlockType type);

    void setSunLight(int x, int y, int z, uint16_t lightLevel);
    void setSunLight(const glm::ivec3& pos, uint16_t lightLevel);
    void setBlockLight(int x, int y, int z, uint16_t lightLevel);
    void setBlockLight(const glm::ivec3& pos, uint16_t lightLevel);

    glm::ivec3 localToGlobalPos(const glm::ivec3& pos);
    static glm::ivec3 localToGlobalPos(const glm::ivec3& pos, const glm::ivec3& chunkPos);
    static glm::ivec3 globalToChunkPos(const glm::ivec3& globalPos);
    static glm::ivec3 globalToLocalPos(const glm::ivec3& globalPos, const glm::ivec3& chunkPos);
    static void globalToLocalPos(const glm::ivec3& globalPos, const glm::ivec3& chunkPos, glm::ivec3& localPosOut, glm::ivec3& chunkPosOut);

private:
    glm::ivec3 m_position{0, 0, 0};
    // organized as x, z, y for cache efficiency in sunlight propagation
    std::array<BlockType, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> m_blocks;
    std::array<uint16_t, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> m_sunLightMap;
    std::array<uint16_t, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> m_blockLightMap;
};