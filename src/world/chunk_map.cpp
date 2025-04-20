#include "world/chunk_map.h"
#include "utils/algorithms.h"

ChunkMap::ChunkMap()
{
}

void ChunkMap::update()
{
    while (!m_chunksToSubmit.empty())
    {
        std::shared_ptr<Chunk> chunk;
        m_chunksToSubmit.pop(chunk);
        auto it = m_chunks.find(chunk->getPos());
        if (it != m_chunks.end()) {
            it->second = chunk;
        } else {
            m_chunks.emplace(chunk->getPos(), chunk);
        }
        m_chunksInBuildQueue.erase(chunk->getPos());
    }
}

void ChunkMap::chunkBuildThreadFunc()
{
    while (!m_stopThread)
    {
        glm::ivec3 chunkPos;
        if (!m_chunksToBuild.popNoWait(chunkPos)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
        auto chunk = std::make_shared<Chunk>(chunkPos);
        m_chunksToSubmit.push(chunk);
    }
}

void ChunkMap::startBuildThread()
{
    m_stopThread = false;
    std::thread([this]() { chunkBuildThreadFunc(); }).detach();
}

void ChunkMap::queueChunk(const glm::ivec3& chunkPos)
{
    if (!m_chunks.contains(chunkPos) && !m_chunksInBuildQueue.contains(chunkPos))
    {
        m_chunksToBuild.push(chunkPos);
        m_chunksInBuildQueue.insert(chunkPos);
    }
}

void ChunkMap::addChunkRadius(const glm::ivec3& chunkPos, int radius) 
{
    std::vector<glm::ivec3> chunksToAdd = algo::getPosFromCenter(chunkPos, radius);
    for (const auto& pos : chunksToAdd)
    {
        if (!m_chunks.contains(pos) && !m_chunksInBuildQueue.contains(pos))
        {
            m_chunksToBuild.push(pos);
            m_chunksInBuildQueue.insert(pos);
        }
    }
}

bool ChunkMap::addChunk(int x, int y, int z)
{
    glm::ivec3 pos(x, y, z);
    if (m_chunks.find(pos) == m_chunks.end())
    {
        m_chunks.emplace(pos, std::make_unique<Chunk>(pos));
        return true;
    }
    return false;
}

bool ChunkMap::addChunk(const glm::ivec3& position)
{
    return addChunk(position.x, position.y, position.z);
}

void ChunkMap::setBlock(int x, int y, int z, BlockType type)
{
    glm::ivec3 pos(x, y, z);
    auto chunkPos = Chunk::globalToChunkPos(pos);
    auto localPos = Chunk::globalToLocalPos(pos);
    auto chunk = getChunkInternal(chunkPos);
    chunk = checkCopy2Write(chunk);
    if (chunk)
    {
        chunk->setBlock(localPos.x, localPos.y, localPos.z, type);
    }
}

void ChunkMap::setBlock(const glm::ivec3& pos, BlockType type)
{
    setBlock(pos.x, pos.y, pos.z, type);
}

void ChunkMap::setBlockLight(int x, int y, int z, uint8_t lightLevel)
{
    glm::ivec3 pos(x, y, z);
    auto chunkPos = Chunk::globalToChunkPos(pos);
    auto localPos = Chunk::globalToLocalPos(pos);
    auto chunk = getChunkInternal(chunkPos);
    chunk = checkCopy2Write(chunk);
    if (chunk)
    {
        chunk->setBlockLight(localPos.x, localPos.y, localPos.z, lightLevel);
    }
}

void ChunkMap::setBlockLight(const glm::ivec3& pos, uint8_t lightLevel)
{
    setBlockLight(pos.x, pos.y, pos.z, lightLevel);
}

void ChunkMap::setSunLight(int x, int y, int z, uint8_t lightLevel)
{
    glm::ivec3 pos(x, y, z);
    auto chunkPos = Chunk::globalToChunkPos(pos);
    auto localPos = Chunk::globalToLocalPos(pos);
    auto chunk = getChunkInternal(chunkPos);
    chunk = checkCopy2Write(chunk);
    if (chunk)
    {
        chunk->setSunLight(localPos.x, localPos.y, localPos.z, lightLevel);
    }
}

void ChunkMap::setSunLight(const glm::ivec3& pos, uint8_t lightLevel)
{
    setSunLight(pos.x, pos.y, pos.z, lightLevel);
}

BlockType ChunkMap::getBlock(int x, int y, int z) const
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

BlockType ChunkMap::getBlock(const glm::ivec3& pos) const
{
    return getBlock(pos.x, pos.y, pos.z);
}

uint16_t ChunkMap::getSunLight(int x, int y, int z) const
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

uint16_t ChunkMap::getSunLight(const glm::ivec3& pos) const
{
    return getSunLight(pos.x, pos.y, pos.z);
}

uint16_t ChunkMap::getBlockLight(int x, int y, int z) const
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

uint16_t ChunkMap::getBlockLight(const glm::ivec3& pos) const
{
    return getBlockLight(pos.x, pos.y, pos.z);
}

uint16_t ChunkMap::getLightLevel(int x, int y, int z) const
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

uint16_t ChunkMap::getLightLevel(const glm::ivec3& pos) const
{
    return getLightLevel(pos.x, pos.y, pos.z);
}

std::shared_ptr<const Chunk> ChunkMap::getChunk(int x, int y, int z) const
{
    glm::ivec3 pos(x, y, z);
    auto it = m_chunks.find(pos);
    if (it != m_chunks.end())
    {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<const Chunk> ChunkMap::getChunk(const glm::ivec3& pos) const
{
    return getChunk(pos.x, pos.y, pos.z);
}

std::vector<std::shared_ptr<const Chunk>> ChunkMap::getChunksInRadius(const glm::ivec3 &chunkPos, int radius) const
{
    std::vector<std::shared_ptr<const Chunk>> chunksInRadius;
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
                }
            }
        }
    }
    return chunksInRadius;
}

std::shared_ptr<Chunk> ChunkMap::getChunkInternal(const glm::ivec3& pos) const
{
    auto it = m_chunks.find(pos);
    if (it != m_chunks.end())
    {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<Chunk> ChunkMap::checkCopy2Write(const std::shared_ptr<Chunk>& chunk)
{
    if (chunk.use_count() > 1)
    {
        auto clone = chunk->clone();
        m_chunks[chunk->getPos()] = clone;
        return clone;
    }
    return chunk;
}