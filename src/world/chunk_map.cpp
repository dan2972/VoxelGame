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
        ChunkSnapshotM snapshot;
        if (!m_chunksToFillLight.popFrontNoWait(snapshot)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
        fillSunLight(snapshot);
        snapshot.center()->m_generationState.store(ChunkGenerationState::Light);
        snapshot.center()->m_inBuildQueue.store(false);
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
            m_chunksToFillLight.pushBack(snapshot.value());
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

void ChunkMap::updateLighting(const glm::ivec3& chunkPos)
{
    std::vector<glm::ivec3> missingChunks;
    auto snapshot = createSnapshotM(chunkPos, &missingChunks, ChunkGenerationState::Blocks);
    if (snapshot) {
        snapshot->center()->m_inBuildQueue.store(true);
        snapshot->center()->m_generationState.store(ChunkGenerationState::Blocks);
        m_chunksToFillLight.pushFront(snapshot.value());
    } else {
        for (const auto& missingChunk : missingChunks) {
            queueChunk(missingChunk);
        }
    }
}

void ChunkMap::fillSunLight(ChunkSnapshotM& snapshot) 
{
    auto center = snapshot.center();

    std::array<int, Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE> sunHeightMap;

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
                    snapshot.setSunLightFromLocalPos(localPos, 15);
                } else {
                    snapshot.setSunLightFromLocalPos(localPos, 0);
                }
                snapshot.setBlockLightFromLocalPos(localPos, 0);
            }
        }
    }
    
    for (int x = 0; x < Chunk::CHUNK_SIZE; ++x)
    {
        for (int z = 0; z < Chunk::CHUNK_SIZE; ++z)
        {
            int height = sunHeightMap[x * Chunk::CHUNK_SIZE + z];
            for (int y = Chunk::CHUNK_SIZE - 1; y >= 0; --y)
            {
                auto localPos = glm::ivec3(x, y, z);
                if (x == 0 || z == 0 || x == Chunk::CHUNK_SIZE - 1 || z == Chunk::CHUNK_SIZE - 1) {
                    auto nbLight = snapshot.getNearbyBlockLight(localPos);
                    if (nbLight > 1)
                        floodFillLightAt(snapshot, localPos, nbLight-1, true);
                }
                
                if (y <= height) {
                    BlockType block = snapshot.getBlockFromLocalPos(localPos);
                    if (!BlockData::isTranslucentBlock(block) && !BlockData::isTransparentBlock(block)) {
                        auto luminosity = BlockData::getLuminosity(block);
                        if (luminosity > 1)
                            floodFillLightAt(snapshot, localPos, luminosity, true);
                        continue;
                    }
                    auto light = snapshot.getNearbySkyLight(localPos);
                    if (light <= 1)
                        continue;
                    
                    floodFillLightAt(snapshot, localPos, light - 1, false);
                }
            }
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

void ChunkMap::floodFillLightAt(ChunkSnapshotM& snapshot, const glm::ivec3& pos, uint16_t value, bool isBlockLight)
{
    auto curLight = isBlockLight ? snapshot.getBlockLightFromLocalPos(pos) :
                                snapshot.getSunLightFromLocalPos(pos);
    if (curLight > value)
        return;
    std::queue<LightQueueNode> queue;
    std::unordered_set<glm::ivec3, glm_ivec3_hash, glm_ivec3_equal> visited;
    queue.push({pos, value});
    visited.insert(pos);
    int count = 0;
    while (!queue.empty())
    {   
        count++;
        auto current = queue.front();
        queue.pop();

        if (isBlockLight)
            snapshot.setBlockLightFromLocalPos(current.pos, current.value);
        else
            snapshot.setSunLightFromLocalPos(current.pos, current.value);
        
        if (current.value == 0)
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
            auto neighborLight = isBlockLight ? snapshot.getBlockLightFromLocalPos(neighborPos) :
                                                snapshot.getSunLightFromLocalPos(neighborPos);
            if (neighborLight < current.value - 1 && !visited.contains(neighborPos)) {
                queue.push({neighborPos, static_cast<uint16_t>(current.value - 1)});
                visited.insert(neighborPos);
            }
        }
    }
}

std::optional<ChunkSnapshotM> ChunkMap::createSnapshotM(const glm::ivec3& centerChunkPos, std::vector<glm::ivec3>* missingChunks, ChunkGenerationState minState)
{
    std::array<std::shared_ptr<Chunk>, 27> chunks;
    bool allChunksLoaded = true;
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            for (int z = -1; z <= 1; ++z)
            {
                auto chunk = getChunkInternal(centerChunkPos + glm::ivec3(x, y, z));
                if (!chunk || chunk->getGenerationState() < minState) {
                    if (missingChunks) {
                        allChunksLoaded = false;
                        missingChunks->push_back(centerChunkPos + glm::ivec3(x, y, z));
                    } else {
                        return std::nullopt;
                    }
                }
                chunks[(x + 1) * 9 + (y + 1) * 3 + (z + 1)] = chunk;
            }
        }
    }
    return allChunksLoaded ? std::optional<ChunkSnapshotM>(chunks) : std::nullopt;
}