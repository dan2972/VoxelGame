#include "world/chunk.h"
#include <queue>
#include <unordered_set>
#include "utils/direction_utils.h"
#include "utils/glm_hash.h"

TerrainGenerator Chunk::s_terrainGenerator;

Chunk::Chunk()
    : m_position(0, 0, 0)
{
}

Chunk::Chunk(const glm::ivec3 &position)
    : m_position(position)
{
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
                setSunLight(x, y, z, 0);
                auto gPos = localToGlobalPos({x, y, z});
                if (gPos.y < noise) {
                    int diff = noise - gPos.y;
                    if (diff < 1) {
                        m_blocks[x * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + y] = BlockType::Grass;
                    } else if (diff < 3) {
                        m_blocks[x * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + y] = BlockType::Dirt;
                    } else {
                        m_blocks[x * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + y] = BlockType::Stone;
                    }
                    if (gPos.y <= 2) {
                        if (diff < 3) {
                            m_blocks[x * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + y] = BlockType::Sand;
                        }
                    }
                    m_allAir = false;
                } 
                else if (gPos.y <= 0) {
                    setSunLight(x, y, z, 15);
                    m_blocks[x * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + y] = BlockType::Water;
                    m_allAir = false;
                }
                else
                {
                    setSunLight(x, y, z, 15);
                    m_blocks[x * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + y] = BlockType::Air;
                    m_allSolid = false;
                }

                setBlockLight(x, y, z, 0);
            }
        }
    }
}

void Chunk::generateLightMap(const ChunkSnapshot& snapshot)
{
    std::array<int, Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE> sunHeightMap;
    std::vector<LightQueueNode> nodes;
    std::vector<LightQueueNode> skyNodes;

    for (int x = 0; x < Chunk::CHUNK_SIZE; ++x)
    {
        for (int z = 0; z < Chunk::CHUNK_SIZE; ++z)
        {
            bool propagateSky = true;
            sunHeightMap[x * Chunk::CHUNK_SIZE + z] = 0;
            for (int y = Chunk::CHUNK_SIZE - 1; y >= 0; --y)
            {
                auto localPos = glm::ivec3(x, y, z);
                BlockType block = snapshot.getBlockFromLocalPos(localPos);

                if (!(BlockData::isTranslucentBlock(block) || BlockData::isTransparentBlock(block)) && propagateSky) {
                    propagateSky = false;
                    sunHeightMap[x * Chunk::CHUNK_SIZE + z] = y;
                }
                
                if (propagateSky) {
                    setSunLight(x, y, z, 15);
                } else {
                    setSunLight(x, y, z, 0);
                }

                if (BlockData::isLuminousBlock(block)) {
                    nodes.push_back({localPos, BlockData::getLuminosity(block)});
                } else if (x == 0 || z == 0 || x == Chunk::CHUNK_SIZE - 1 || z == Chunk::CHUNK_SIZE - 1) {
                    auto nbLight = snapshot.getNearbyBlockLight(localPos);
                    if (nbLight > 1)
                        nodes.push_back({localPos, static_cast<uint16_t>(nbLight - 1)});
                }

                setBlockLight(x, y, z, 0);
            }
        }
    }
    
    for (int x = 0; x < Chunk::CHUNK_SIZE; ++x)
    {
        for (int z = 0; z < Chunk::CHUNK_SIZE; ++z)
        {
            int height = sunHeightMap[x * Chunk::CHUNK_SIZE + z];
            for (int y = height; y >= 0; --y)
            {
                auto block = getBlock(x, y, z);
                auto localPos = glm::ivec3(x, y, z);
                
                if (!BlockData::isTranslucentBlock(block) && !BlockData::isTransparentBlock(block)) {
                    continue;
                }
                auto light = snapshot.getNearbySkyLight(localPos);
                if (light <= 1)
                    continue;
                
                skyNodes.push_back({localPos, static_cast<uint16_t>(light - 1)});
            }
        }
    }

    floodFillLightAt(snapshot, skyNodes, false);
    floodFillLightAt(snapshot, nodes, true);
}

BlockType Chunk::getBlock(int x, int y, int z) const
{
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
    {
        return BlockType::Air;
    }
    return m_blocks[x * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + y];
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
    return (m_lightMap[x * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + y] >> 12) & 0xF;
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
    return (m_lightMap[x * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + y] >> 8) & 0xF;
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
    m_blocks[x * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + y] = type;
    if (type != BlockType::Air)
        m_allAir = false;
    else
        m_allSolid = false;
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
    uint16_t &light = m_lightMap[x * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + y];
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
    uint16_t &light = m_lightMap[x * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + y];
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
    chunk->m_generationState = m_generationState.load();
    chunk->m_inBuildQueue = m_inBuildQueue.load();
    return chunk;
}

void Chunk::floodFillLightAt(const ChunkSnapshot& snapshot, const std::vector<LightQueueNode>& nodes, bool isBlockLight)
{
    std::queue<LightQueueNode> queue;
    for (const auto& node : nodes)
    {
        if (isBlockLight && node.value <= getBlockLight(node.pos))
            continue;
        queue.push(node);
        if (isBlockLight)
            setBlockLight(node.pos, node.value);
        else
            setSunLight(node.pos, node.value);
    }

    while (!queue.empty())
    {
        auto current = queue.front();
        queue.pop();
        
        if (current.value <= 1)
            continue;

        if (isBlockLight && current.value < getBlockLight(current.pos))
            continue;

        for (int i = 0; i < 6; ++i)
        {
            glm::ivec3 dir = static_cast<glm::ivec3>(DirectionUtils::blockfaceDirection(static_cast<BlockFace>(i)));
            glm::ivec3 neighborPos = current.pos + dir;
            if (!ChunkSnapshot::inCenterBounds(neighborPos))
                continue;

            auto block = snapshot.getBlockFromLocalPos(neighborPos);
            if (!BlockData::isTranslucentBlock(block) && !BlockData::isTransparentBlock(block))
                continue;
            
            if (isBlockLight) {
                auto neighborLight = snapshot.getBlockLightFromLocalPos(neighborPos);
                if (neighborLight < current.value - 1) {
                    queue.push({neighborPos, static_cast<uint16_t>(current.value - 1)});
                    setBlockLight(neighborPos, current.value-1);
                }
            } else {
                auto neighborLight = snapshot.getSunLightFromLocalPos(neighborPos);
                if (current.value == 15 && BlockFace(i) == BlockFace::Bottom) {
                    queue.push({neighborPos, static_cast<uint16_t>(current.value)});
                    setSunLight(neighborPos, current.value);
                } else if (neighborLight < current.value - 1) {
                    queue.push({neighborPos, static_cast<uint16_t>(current.value - 1)});
                    setSunLight(neighborPos, current.value-1);
                }
            }
        }
    }
}