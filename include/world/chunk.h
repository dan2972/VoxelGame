#pragma once

#include <glm/glm.hpp>
#include <array>
#include <memory>
#include <atomic>
#include <cstdint>
#include <vector>
#include "terrain_generator.h"
#include "block_data.h"
#include "world/chunk_snapshot.h"
#include "world/chunk_generation_state.h"

struct LightQueueNode
{
    glm::ivec3 pos;
    uint16_t value;
};

struct LightRemoveNode
{
    glm::ivec3 pos;
    uint16_t value;
    uint16_t prevVal;
};

class Chunk
{
public:
    friend class ChunkMap;
    const static int CHUNK_SIZE = 32;

    Chunk();
    Chunk(const glm::ivec3& position);
    ~Chunk() = default;

    void generateTerrain();
    void generateLightMap(ChunkSnapshotM& snapshot);
    void floodFillLightAt(ChunkSnapshotM& snapshot, const std::vector<LightQueueNode>& nodes, bool isBlockLight);
    std::vector<LightQueueNode> floodRemoveLightAt(ChunkSnapshotM& snapshot, const std::vector<LightQueueNode>& nodes, bool isBlockLight);
    void clearLightMap();

    glm::ivec3 getPos() const { return m_position; }

    ChunkGenerationState getGenerationState() const { return m_generationState.load(); }

    BlockType getBlock(int x, int y, int z) const;
    BlockType getBlock(const glm::ivec3& pos) const;

    uint16_t getSunLight(int x, int y, int z) const;
    uint16_t getSunLight(const glm::ivec3& pos) const;
    uint16_t getBlockLight(int x, int y, int z) const;
    uint16_t getBlockLight(const glm::ivec3& pos) const;

    uint16_t getLightLevel(int x, int y, int z) const;
    uint16_t getLightLevel(const glm::ivec3& pos) const;

    void setBlock(int x, int y, int z, BlockType type);
    void setBlock(const glm::ivec3& pos, BlockType type);

    void setSunLight(int x, int y, int z, uint8_t lightLevel);
    void setSunLight(const glm::ivec3& pos, uint8_t lightLevel);
    void setBlockLight(int x, int y, int z, uint8_t lightLevel);
    void setBlockLight(const glm::ivec3& pos, uint8_t lightLevel);

    glm::ivec3 localToGlobalPos(const glm::ivec3& pos);
    static glm::ivec3 localToGlobalPos(const glm::ivec3& pos, const glm::ivec3& chunkPos);
    static glm::ivec3 globalToChunkPos(const glm::ivec3& globalPos);
    static glm::ivec3 globalToLocalPos(const glm::ivec3& globalPos);
    static void globalToLocalPos(const glm::ivec3& globalPos, glm::ivec3& localPosOut, glm::ivec3& chunkPosOut);

    bool isAllAir() const { return m_allAir; }
    bool isAllSolid() const { return m_allSolid; }

    std::shared_ptr<Chunk> clone() const;
private:
    static TerrainGenerator s_terrainGenerator;
    glm::ivec3 m_position{0, 0, 0};
    // organized as x, z, y for cache efficiency in sunlight propagation
    std::array<BlockType, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> m_blocks;
    std::array<uint16_t, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> m_lightMap;

    bool m_allAir = true;
    bool m_allSolid = true;
    std::atomic<bool> m_inBuildQueue = false;
    std::atomic<ChunkGenerationState> m_generationState = ChunkGenerationState::None;
};