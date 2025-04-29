#include "world/chunk_map.h"
#include "utils/algorithms.h"
#include "utils/direction_utils.h"
#include "world/chunk_snapshot.h"

ChunkMap::ChunkMap()
{
}

void ChunkMap::update()
{

}

void ChunkMap::chunkLightThreadFunc()
{
    while (!m_stopThread)
    {
        ChunkLightQueueNode node;
        if (!m_chunksToFillLight.popFrontNoWait(node)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
        auto center = getChunkInternal(node.snapshot.center()->getPos());
        if (node.clearLightMap) {
            center->clearLightMap();
        } else {
            center->generateLightMap(node.snapshot);
        }
        center->m_generationState.store(ChunkGenerationState::Light);
        center->m_inBuildQueue.store(false);
    }
}

void ChunkMap::chunkBuildThreadFunc()
{
    while (!m_stopThread)
    {
        std::shared_ptr<Chunk> chunk;
        if (!m_chunksToBuild.popFrontNoWait(chunk)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
        chunk->generateTerrain();
        if (chunk->isAllAir() || chunk->isAllSolid())
            chunk->m_generationState.store(ChunkGenerationState::Light);
        else
            chunk->m_generationState.store(ChunkGenerationState::Blocks);
        chunk->m_inBuildQueue.store(false);
    }
}

void ChunkMap::startBuildThread()
{
    m_stopThread = false;
    std::thread([this]() { chunkBuildThreadFunc(); }).detach();
    std::thread([this]() { chunkLightThreadFunc(); }).detach();
}

void ChunkMap::queueChunk(const glm::ivec3& chunkPos)
{
    auto chunk = getChunkInternal(chunkPos);
    if (!chunk) 
    {
        auto newChunk = std::make_shared<Chunk>(chunkPos);
        newChunk->m_inBuildQueue.store(true);
        m_chunksToBuild.pushBack(newChunk);
        m_chunks[chunkPos] = newChunk;
    } else if (chunk->getGenerationState() == ChunkGenerationState::None && !chunk->m_inBuildQueue.load()) {
        chunk->m_inBuildQueue.store(true);
        m_chunksToBuild.pushBack(chunk);
    } else if (chunk->getGenerationState() == ChunkGenerationState::Blocks && !chunk->m_inBuildQueue.load()) {
        std::vector<glm::ivec3> missingChunks;
        auto snapshot = createSnapshotM(chunkPos, &missingChunks, ChunkGenerationState::Blocks);
        if (snapshot) {
            chunk->m_inBuildQueue.store(true);
            m_chunksToFillLight.pushBack({snapshot.value(), false});
        } else {
            for (const auto& missingChunk : missingChunks) {
                queueChunk(missingChunk);
            }
        }
    }
}

void ChunkMap::queueChunkRadius(const glm::ivec3 &chunkPos, int radius)
{
    for (int x = -radius; x <= radius; ++x)
    {
        for (int y = -radius; y <= radius; ++y)
        {
            for (int z = -radius; z <= radius; ++z)
            {
                queueChunk(chunkPos + glm::ivec3(x, y, z));
            }
        }
    }
}

void ChunkMap::setBlock(int x, int y, int z, BlockType type)
{
    glm::ivec3 pos(x, y, z);
    auto chunkPos = Chunk::globalToChunkPos(pos);
    auto localPos = Chunk::globalToLocalPos(pos);
    auto chunk = getChunkInternal(chunkPos);
    chunk = checkCopy2Write(chunk);
    if (chunk->getGenerationState() < ChunkGenerationState::Complete)
        return;
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
    if (chunk->getGenerationState() < ChunkGenerationState::Complete)
        return;
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
    if (chunk->getGenerationState() < ChunkGenerationState::Complete)
        return;
    if (chunk)
    {
        chunk->setSunLight(localPos.x, localPos.y, localPos.z, lightLevel);
    }
}

void ChunkMap::setSunLight(const glm::ivec3& pos, uint8_t lightLevel)
{
    setSunLight(pos.x, pos.y, pos.z, lightLevel);
}

void ChunkMap::addLights(const glm::ivec3& chunkPos, const std::vector<LightQueueNode>& nodes, bool isBlockLight) {
    std::vector<glm::ivec3> missingChunks;
    auto snapshot = createSnapshotM(chunkPos, &missingChunks, ChunkGenerationState::Blocks);
    if (snapshot) {
        auto localNodes = nodes;
        for (auto& node : localNodes) {
            node.pos = Chunk::globalToLocalPos(node.pos);
        }
        snapshot->center()->floodFillLightAt(snapshot.value(), localNodes, isBlockLight);
    } else {
        for (const auto& missingChunk : missingChunks) {
            queueChunk(missingChunk);
        }
    }
}

void ChunkMap::removeLights(const glm::ivec3& chunkPos, const std::vector<LightQueueNode>& nodes, bool isBlockLight) {
    std::vector<glm::ivec3> missingChunks;
    auto snapshot = createSnapshotM(chunkPos, &missingChunks, ChunkGenerationState::Blocks);
    if (snapshot) {
        auto localNodes = nodes;
        for (auto& node : localNodes) {
            node.pos = Chunk::globalToLocalPos(node.pos);
        }
        auto toAdd = snapshot->center()->floodRemoveLightAt(snapshot.value(), localNodes, isBlockLight);
        snapshot->center()->floodFillLightAt(snapshot.value(), toAdd, isBlockLight);
    } else {
        for (const auto& missingChunk : missingChunks) {
            queueChunk(missingChunk);
        }
    }
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

uint16_t ChunkMap::getNearbySkyLight(const glm::ivec3& pos) const
{
    auto snapshot = ChunkSnapshot::CreateSnapshot(*this, Chunk::globalToChunkPos(pos), ChunkGenerationState::Light);
    if (snapshot) {
        return snapshot->getNearbySkyLight(Chunk::globalToLocalPos(pos));
    }
    return 0;
}

uint16_t ChunkMap::getNearbyBlockLight(const glm::ivec3& pos) const
{
    auto snapshot = ChunkSnapshot::CreateSnapshot(*this, Chunk::globalToChunkPos(pos), ChunkGenerationState::Light);
    if (snapshot) {
        return snapshot->getNearbyBlockLight(Chunk::globalToLocalPos(pos));
    }
    return 0;
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

std::optional<ChunkSnapshotM> ChunkMap::createSnapshotM(const glm::ivec3& centerChunkPos, std::vector<glm::ivec3>* missingChunks, ChunkGenerationState minState) const
{
    bool allChunksLoaded = true;
    auto centerChunk = getChunkInternal(centerChunkPos);
    if (!centerChunk || centerChunk->getGenerationState() < minState) {
        missingChunks->push_back(centerChunkPos);
        allChunksLoaded = false;
    }
    
    ChunkSnapshotM snapshot;
    snapshot.chunks[13] = centerChunk;
    for (const auto& dir : ChunkSnapshot::getRequiredChunkDirs()) {
        auto chunk = getChunkInternal(centerChunkPos + dir);
        if (!chunk || chunk->getGenerationState() < minState) {
            missingChunks->push_back(centerChunkPos + dir);
            allChunksLoaded = false;
            continue;
        }
        snapshot.chunks[(dir.x + 1) * 9 + (dir.y + 1) * 3 + (dir.z + 1)] = chunk;
    }
    return allChunksLoaded ? std::optional<ChunkSnapshotM>(snapshot) : std::nullopt;
}