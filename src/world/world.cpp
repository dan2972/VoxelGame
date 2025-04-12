#include "world/world.h"

World::World()
{
}

void World::update()
{
    int meshBuildCount = 0;
    while (!m_chunkUpdateQueue.empty())
    {
        auto chunkPos = m_chunkUpdateQueue.top().chunkPos;
        m_chunkUpdateQueue.pop();
        if (meshBuildCount < 16 && addChunk(chunkPos)) {
            meshBuildCount++;
        }
    }
}

void World::addChunkRadius(const glm::ivec3& chunkPos, int radius) 
{
    for (int x = -radius; x <= radius; ++x)
    {
        for (int y = -radius; y <= radius; ++y)
        {
            for (int z = -radius; z <= radius; ++z)
            {
                int dx = x - chunkPos.x;
                int dy = y - chunkPos.y;
                int dz = z - chunkPos.z;
                int distance = dx * dx + dy * dy + dz * dz;
                glm::ivec3 pos = chunkPos + glm::ivec3(x, y, z);
                if (!m_chunks.contains(pos))
                {
                    m_chunkUpdateQueue.push(ChunkQueueNode{pos, distance});
                }
            }
        }
    }
}

bool World::addChunk(int x, int y, int z)
{
    glm::ivec3 pos(x, y, z);
    if (m_chunks.find(pos) == m_chunks.end())
    {
        m_chunks.emplace(pos, std::make_unique<Chunk>(pos));
        return true;
    }
    return false;
}

bool World::addChunk(const glm::ivec3& position)
{
    return addChunk(position.x, position.y, position.z);
}

void World::setBlock(int x, int y, int z, BlockType type)
{
    glm::ivec3 pos(x, y, z);
    auto chunkPos = Chunk::globalToChunkPos(pos);
    auto localPos = Chunk::globalToLocalPos(pos);
    auto chunk = getChunk(chunkPos);
    if (chunk)
    {
        chunk->setBlock(localPos.x, localPos.y, localPos.z, type);
    }
}

void World::setBlock(const glm::ivec3& pos, BlockType type)
{
    setBlock(pos.x, pos.y, pos.z, type);
}

void World::setBlockLight(int x, int y, int z, uint16_t lightLevel)
{
    glm::ivec3 pos(x, y, z);
    auto chunkPos = Chunk::globalToChunkPos(pos);
    auto localPos = Chunk::globalToLocalPos(pos);
    auto chunk = getChunk(chunkPos);
    if (chunk)
    {
        chunk->setBlockLight(localPos.x, localPos.y, localPos.z, lightLevel);
    }
}

void World::setBlockLight(const glm::ivec3& pos, uint16_t lightLevel)
{
    setBlockLight(pos.x, pos.y, pos.z, lightLevel);
}

void World::setSunLight(int x, int y, int z, uint16_t lightLevel)
{
    glm::ivec3 pos(x, y, z);
    auto chunkPos = Chunk::globalToChunkPos(pos);
    auto localPos = Chunk::globalToLocalPos(pos);
    auto chunk = getChunk(chunkPos);
    if (chunk)
    {
        chunk->setSunLight(localPos.x, localPos.y, localPos.z, lightLevel);
    }
}

void World::setSunLight(const glm::ivec3& pos, uint16_t lightLevel)
{
    setSunLight(pos.x, pos.y, pos.z, lightLevel);
}

BlockType World::getBlock(int x, int y, int z) const
{
    glm::ivec3 pos(x, y, z);
    glm::ivec3 chunkPos = Chunk::globalToChunkPos(pos);
    glm::ivec3 localPos = Chunk::globalToLocalPos(pos);
    auto chunk = getChunk(chunkPos);
    if (chunk)
    {
        return chunk->getBlock(localPos.x, localPos.y, localPos.z);
    }
    return BlockType::Air;
}

BlockType World::getBlock(const glm::ivec3& pos) const
{
    return getBlock(pos.x, pos.y, pos.z);
}

uint16_t World::getSunLight(int x, int y, int z) const
{
    glm::ivec3 pos(x, y, z);
    glm::ivec3 chunkPos = Chunk::globalToChunkPos(pos);
    glm::ivec3 localPos = Chunk::globalToLocalPos(pos);
    auto chunk = getChunk(chunkPos);
    if (chunk)
    {
        return chunk->getSunLight(localPos.x, localPos.y, localPos.z);
    }
    return 15;
}

uint16_t World::getSunLight(const glm::ivec3& pos) const
{
    return getSunLight(pos.x, pos.y, pos.z);
}

uint16_t World::getBlockLight(int x, int y, int z) const
{
    glm::ivec3 pos(x, y, z);
    glm::ivec3 chunkPos = Chunk::globalToChunkPos(pos);
    glm::ivec3 localPos = Chunk::globalToLocalPos(pos);
    auto chunk = getChunk(chunkPos);
    if (chunk)
    {
        return chunk->getBlockLight(localPos.x, localPos.y, localPos.z);
    }
    return 0;
}

uint16_t World::getBlockLight(const glm::ivec3& pos) const
{
    return getBlockLight(pos.x, pos.y, pos.z);
}

uint16_t World::getLightLevel(int x, int y, int z) const
{
    glm::ivec3 pos(x, y, z);
    glm::ivec3 chunkPos = Chunk::globalToChunkPos(pos);
    glm::ivec3 localPos = Chunk::globalToLocalPos(pos);
    auto chunk = getChunk(chunkPos);
    if (chunk)
    {
        return std::max(chunk->getBlockLight(localPos), chunk->getSunLight(localPos));
    }
    return 15;
}

uint16_t World::getLightLevel(const glm::ivec3& pos) const
{
    return getLightLevel(pos.x, pos.y, pos.z);
}

Chunk* World::getChunk(int x, int y, int z) const
{
    glm::ivec3 pos(x, y, z);
    auto it = m_chunks.find(pos);
    if (it != m_chunks.end())
    {
        return it->second.get();
    }
    return nullptr;
}

Chunk* World::getChunk(const glm::ivec3& pos) const
{
    return getChunk(pos.x, pos.y, pos.z);
}

std::vector<Chunk *> World::getChunksInRadius(const glm::ivec3 &chunkPos, int radius) const
{
    std::vector<Chunk *> chunksInRadius;
    for (int x = chunkPos.x - radius; x <= chunkPos.x + radius; ++x)
    {
        for (int y = chunkPos.y - radius; y <= chunkPos.y + radius; ++y)
        {
            for (int z = chunkPos.z - radius; z <= chunkPos.z + radius; ++z)
            {
                int dx = x - chunkPos.x;
                int dy = y - chunkPos.y;
                int dz = z - chunkPos.z;
                if (dx * dx + dy * dy + dz * dz > radius * radius)
                    continue;
                auto chunk = getChunk(x, y, z);
                if (chunk)
                {
                    chunksInRadius.push_back(chunk);
                }else {
                    printf("Chunk not found at (%d, %d, %d)\n", x, y, z);
                }
            }
        }
    }
    return chunksInRadius;
}