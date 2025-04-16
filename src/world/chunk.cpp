#include "world/chunk.h"

TerrainGenerator Chunk::s_terrainGenerator;

Chunk::Chunk()
    : m_position(0, 0, 0)
{
    generateTerrain();
}

Chunk::Chunk(const glm::ivec3 &position)
    : m_position(position)
{
    generateTerrain();
}

void Chunk::generateTerrain()
{
    for (int x = 0; x < CHUNK_SIZE; ++x)
    {
        for (int z = 0; z < CHUNK_SIZE; ++z)
        {
            glm::ivec3 globalPos = localToGlobalPos({x, 0, z});
            float noise = s_terrainGenerator.getNoise(globalPos.x, globalPos.z) * 256 + 6;
            for (int y = 0; y < CHUNK_SIZE; ++y)
            {
                auto gPos = localToGlobalPos({x, y, z});
                if (gPos.y < noise) {
                    int diff = noise - gPos.y;
                    if (diff < 1) {
                        m_blocks[x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE] = BlockType::Grass;
                    } else if (diff < 3) {
                        m_blocks[x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE] = BlockType::Dirt;
                    } else {
                        m_blocks[x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE] = BlockType::Stone;
                    }
                    if (gPos.y <= 2) {
                        if (diff < 3) {
                            m_blocks[x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE] = BlockType::Sand;
                        }
                    }
                    m_allAir = false;
                } 
                else if (gPos.y <= 0) {
                    m_blocks[x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE] = BlockType::Water;
                    m_allAir = false;
                }
                else
                {
                    m_blocks[x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE] = BlockType::Air;
                    m_allSolid = false;
                }
                
                setSunLight(x, y, z, 15);
                setBlockLight(x, y, z, 0);
            }
        }
    }
}

BlockType Chunk::getBlock(int x, int y, int z) const
{
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
    {
        return BlockType::Air;
    }
    return m_blocks[x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE];
}

BlockType Chunk::getBlock(const glm::ivec3 &pos) const
{
    return getBlock(pos.x, pos.y, pos.z);
}

uint16_t Chunk::getSunLight(int x, int y, int z) const
{
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
    {
        return 15;
    }
    return (m_lightMap[x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE] >> 12) & 0xF;
}

uint16_t Chunk::getSunLight(const glm::ivec3 &pos) const
{
    return getSunLight(pos.x, pos.y, pos.z);
}

uint16_t Chunk::getBlockLight(int x, int y, int z) const
{
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
    {
        return 0;
    }
    return (m_lightMap[x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE] >> 8) & 0xF;
}

uint16_t Chunk::getBlockLight(const glm::ivec3 &pos) const
{
    return getBlockLight(pos.x, pos.y, pos.z);
}

uint16_t Chunk::getLightLevel(int x, int y, int z) const
{
    return std::max(getBlockLight(x, y, z), getSunLight(x, y, z));
}

uint16_t Chunk::getLightLevel(const glm::ivec3 &pos) const
{
    return getLightLevel(pos.x, pos.y, pos.z);
}

void Chunk::setBlock(int x, int y, int z, BlockType type)
{
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
    {
        return;
    }
    m_blocks[x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE] = type;
    if (type != BlockType::Air)
        m_allAir = false;
}

void Chunk::setBlock(const glm::ivec3 &pos, BlockType type)
{
    setBlock(pos.x, pos.y, pos.z, type);
}

void Chunk::setSunLight(int x, int y, int z, uint8_t lightLevel)
{
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
    {
        return;
    }
    uint16_t &light = m_lightMap[x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE];
    light &= ~(0xF << 12);
    light |= ((lightLevel & 0xF) << 12);
}

void Chunk::setSunLight(const glm::ivec3 &pos, uint8_t lightLevel)
{
    setSunLight(pos.x, pos.y, pos.z, lightLevel);
}

void Chunk::setBlockLight(int x, int y, int z, uint8_t lightLevel)
{
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
    {
        return;
    }
    uint16_t &light = m_lightMap[x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE];
    light &= ~(0xF << 8);
    light |= ((lightLevel & 0xF) << 8);
}

void Chunk::setBlockLight(const glm::ivec3 &pos, uint8_t lightLevel)
{
    setBlockLight(pos.x, pos.y, pos.z, lightLevel);
}

glm::ivec3 Chunk::localToGlobalPos(const glm::ivec3 &pos)
{
    int x = m_position.x * CHUNK_SIZE + pos.x;
    int y = m_position.y * CHUNK_SIZE + pos.y;
    int z = m_position.z * CHUNK_SIZE + pos.z;
    return {x, y, z};
}

glm::ivec3 Chunk::localToGlobalPos(const glm::ivec3 &pos, const glm::ivec3 &chunkPos)
{
    int x = chunkPos.x * CHUNK_SIZE + pos.x;
    int y = chunkPos.y * CHUNK_SIZE + pos.y;
    int z = chunkPos.z * CHUNK_SIZE + pos.z;
    return {x, y, z};
}

glm::ivec3 Chunk::globalToChunkPos(const glm::ivec3 &globalPos)
{
    glm::ivec3 out;
    out.x = globalPos.x >= 0 ? globalPos.x / CHUNK_SIZE : (globalPos.x + 1) / CHUNK_SIZE - 1;
    out.y = globalPos.y >= 0 ? globalPos.y / CHUNK_SIZE : (globalPos.y + 1) / CHUNK_SIZE - 1;
    out.z = globalPos.z >= 0 ? globalPos.z / CHUNK_SIZE : (globalPos.z + 1) / CHUNK_SIZE - 1;
    return out;
}

glm::ivec3 Chunk::globalToLocalPos(const glm::ivec3 &globalPos)
{
    int localX = globalPos.x % CHUNK_SIZE;
    localX = localX < 0 ? CHUNK_SIZE + localX : localX;
    int localY = globalPos.y % CHUNK_SIZE;
    localY = localY < 0 ? CHUNK_SIZE + localY : localY;
    int localZ = globalPos.z % CHUNK_SIZE;
    localZ = localZ < 0 ? CHUNK_SIZE + localZ : localZ;
    return {localX, localY, localZ};
}

void Chunk::globalToLocalPos(const glm::ivec3 &globalPos, glm::ivec3 &localPosOut, glm::ivec3 &chunkPosOut)
{
    chunkPosOut = globalToChunkPos(globalPos);
    int localX = globalPos.x % CHUNK_SIZE;
    localX = localX < 0 ? CHUNK_SIZE + localX : localX;
    int localY = globalPos.y % CHUNK_SIZE;
    localY = localY < 0 ? CHUNK_SIZE + localY : localY;
    int localZ = globalPos.z % CHUNK_SIZE;
    localZ = localZ < 0 ? CHUNK_SIZE + localZ : localZ;
    localPosOut = {localX, localY, localZ};
}

std::shared_ptr<Chunk> Chunk::clone() const
{
    auto chunk = std::make_shared<Chunk>(m_position);
    chunk->m_blocks = m_blocks;
    chunk->m_lightMap = m_lightMap;
    chunk->m_allAir = m_allAir;
    chunk->m_allSolid = m_allSolid;
    return chunk;
}