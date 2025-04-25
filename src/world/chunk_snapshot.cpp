#include "world/chunk_snapshot.h"
#include "world/chunk_map.h"
#include "utils/direction_utils.h"

ChunkSnapshot::ChunkSnapshot() {
    std::fill(chunks.begin(), chunks.end(), nullptr);
}

std::optional<ChunkSnapshot> ChunkSnapshot::CreateSnapshot(const ChunkMap& chunkMap, const glm::ivec3& centerChunkPos, ChunkGenerationState minState) {
    auto centerChunk = chunkMap.getChunk(centerChunkPos);
    if (!centerChunk || centerChunk->getGenerationState() < minState)
        return std::nullopt;
    ChunkSnapshot snapshot;
    snapshot.chunks[13] = centerChunk;
    for (const auto& dir : getRequiredChunkDirs()) {
        auto chunk = chunkMap.getChunk(centerChunkPos + dir);
        if (!chunk || chunk->getGenerationState() < minState)
            return std::nullopt;
        snapshot.chunks[(dir.x + 1) * 9 + (dir.y + 1) * 3 + (dir.z + 1)] = chunk;
    }
    return snapshot;
}

std::optional<ChunkSnapshot> ChunkSnapshot::CreateSnapshot(const ChunkMap &chunkMap, const glm::ivec3 &centerChunkPos, std::vector<glm::ivec3>* missingChunks, ChunkGenerationState minState)
{
    bool allChunksLoaded = true;
    auto centerChunk = chunkMap.getChunk(centerChunkPos);
    if (!centerChunk || centerChunk->getGenerationState() < minState) {
        missingChunks->push_back(centerChunkPos);
        allChunksLoaded = false;
    }
    
    ChunkSnapshot snapshot;
    snapshot.chunks[13] = centerChunk;
    for (const auto& dir : getRequiredChunkDirs()) {
        auto chunk = chunkMap.getChunk(centerChunkPos + dir);
        if (!chunk || chunk->getGenerationState() < minState) {
            missingChunks->push_back(centerChunkPos + dir);
            allChunksLoaded = false;
            continue;
        }
        snapshot.chunks[(dir.x + 1) * 9 + (dir.y + 1) * 3 + (dir.z + 1)] = chunk;
    }
    return allChunksLoaded ? std::optional<ChunkSnapshot>(snapshot) : std::nullopt;
}

std::shared_ptr<const Chunk> ChunkSnapshot::center() const {
    return chunks[13];
}

std::vector<glm::ivec3> ChunkSnapshot::getRequiredChunkDirs() {
    std::vector<glm::ivec3> dirs;
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            for (int z = -1; z <= 1; ++z) {
                if (x == 0 && y == 0 && z == 0) continue;
                dirs.emplace_back(x, y, z);
            }
        }
    }
    return dirs;
}

// returns the chunk at a given local position.
// local position is relative to the origin of the center chunk.
std::shared_ptr<const Chunk> ChunkSnapshot::getChunkFromLocalPos(const glm::ivec3& localPos) const {
    glm::ivec3 chunkPos = getRelChunkPosFromLocalPos(localPos);
    int index = (chunkPos.x + 1) * 9 + (chunkPos.y + 1) * 3 + (chunkPos.z + 1);
    return chunks[index];
}

BlockType ChunkSnapshot::getBlockFromLocalPos(const glm::ivec3& localPos) const {
    if (inCenterBounds(localPos)) {
        return center()->getBlock(localPos);
    }
    auto chunk = getChunkFromLocalPos(localPos);
    if (chunk) {
        glm::ivec3 innerLocalPos = (localPos + Chunk::CHUNK_SIZE) % Chunk::CHUNK_SIZE;
        return chunk->getBlock(innerLocalPos);
    }
    return BlockType::Air;
}

uint16_t ChunkSnapshot::getSunLightFromLocalPos(const glm::ivec3& localPos) const {
    if (inCenterBounds(localPos)) {
        return center()->getSunLight(localPos);
    }
    auto chunk = getChunkFromLocalPos(localPos);
    if (chunk) {
        glm::ivec3 innerLocalPos = (localPos + Chunk::CHUNK_SIZE) % Chunk::CHUNK_SIZE;
        return chunk->getSunLight(innerLocalPos);
    }
    return 0;
}

uint16_t ChunkSnapshot::getBlockLightFromLocalPos(const glm::ivec3& localPos) const {
    if (inCenterBounds(localPos)) {
        return center()->getBlockLight(localPos);
    }
    auto chunk = getChunkFromLocalPos(localPos);
    if (chunk) {
        glm::ivec3 innerLocalPos = (localPos + Chunk::CHUNK_SIZE) % Chunk::CHUNK_SIZE;
        return chunk->getBlockLight(innerLocalPos);
    }
    return 0;
}

uint16_t ChunkSnapshot::getLightLevelFromLocalPos(const glm::ivec3& localPos) const {
    if (inCenterBounds(localPos)) {
        return center()->getLightLevel(localPos);
    }
    auto chunk = getChunkFromLocalPos(localPos);
    if (chunk) {
        glm::ivec3 innerLocalPos = (localPos + Chunk::CHUNK_SIZE) % Chunk::CHUNK_SIZE;
        return chunk->getLightLevel(innerLocalPos);
    }
    return 15;
}

uint16_t ChunkSnapshot::getNearbySkyLight(const glm::ivec3 &localPos) const
{
    uint16_t maxLight = 0;
    for (int i = 0; i < 6; ++i) {
        glm::ivec3 dir = static_cast<glm::ivec3>(DirectionUtils::blockfaceDirection(static_cast<BlockFace>(i)));
        glm::ivec3 neighborPos = localPos + dir;
        if (neighborPos.x < -Chunk::CHUNK_SIZE || neighborPos.x >= Chunk::CHUNK_SIZE * 2 ||
            neighborPos.y < -Chunk::CHUNK_SIZE || neighborPos.y >= Chunk::CHUNK_SIZE * 2 ||
            neighborPos.z < -Chunk::CHUNK_SIZE || neighborPos.z >= Chunk::CHUNK_SIZE * 2) {
            continue; // Skip out of bounds neighbors
        }
        auto light = getSunLightFromLocalPos(neighborPos);
        maxLight = std::max(maxLight, light);
    }
    return maxLight;
}

uint16_t ChunkSnapshot::getNearbyBlockLight(const glm::ivec3 &localPos) const
{
    uint16_t maxLight = 0;
    for (int i = 0; i < 6; ++i) {
        glm::ivec3 dir = static_cast<glm::ivec3>(DirectionUtils::blockfaceDirection(static_cast<BlockFace>(i)));
        glm::ivec3 neighborPos = localPos + dir;
        if (neighborPos.x < -Chunk::CHUNK_SIZE || neighborPos.x >= Chunk::CHUNK_SIZE * 2 ||
            neighborPos.y < -Chunk::CHUNK_SIZE || neighborPos.y >= Chunk::CHUNK_SIZE * 2 ||
            neighborPos.z < -Chunk::CHUNK_SIZE || neighborPos.z >= Chunk::CHUNK_SIZE * 2) {
            continue; // Skip out of bounds neighbors
        }
        auto light = getBlockLightFromLocalPos(neighborPos);
        maxLight = std::max(maxLight, light);
    }
    return maxLight;
}

bool ChunkSnapshot::isValid(ChunkGenerationState minState) const {
    return std::all_of(chunks.begin(), chunks.end(), [&](const auto& chunk) { return chunk != nullptr && chunk->getGenerationState() >= minState; });
}

glm::ivec3 ChunkSnapshot::getRelChunkPosFromLocalPos(const glm::ivec3& localPos) {
    return {
        localPos.x < 0 ? -1 : (localPos.x >= Chunk::CHUNK_SIZE ? 1 : 0),
        localPos.y < 0 ? -1 : (localPos.y >= Chunk::CHUNK_SIZE ? 1 : 0),
        localPos.z < 0 ? -1 : (localPos.z >= Chunk::CHUNK_SIZE ? 1 : 0)
    };
}

bool ChunkSnapshot::inCenterBounds(const glm::ivec3& localPos) {
    return localPos.x >= 0 && localPos.x < Chunk::CHUNK_SIZE &&
           localPos.z >= 0 && localPos.z < Chunk::CHUNK_SIZE &&
           localPos.y >= 0 && localPos.y < Chunk::CHUNK_SIZE;
}

// chunk snapshot modifiable version

std::shared_ptr<Chunk> ChunkSnapshotM::getChunkFromLocalPos(const glm::ivec3& localPos) {
    glm::ivec3 chunkPos = ChunkSnapshot::getRelChunkPosFromLocalPos(localPos);
    int index = (chunkPos.x + 1) * 9 + (chunkPos.y + 1) * 3 + (chunkPos.z + 1);
    return chunks[index];
}

std::shared_ptr<const Chunk> ChunkSnapshotM::getChunkFromLocalPos(const glm::ivec3& localPos) const {
    glm::ivec3 chunkPos = ChunkSnapshot::getRelChunkPosFromLocalPos(localPos);
    int index = (chunkPos.x + 1) * 9 + (chunkPos.y + 1) * 3 + (chunkPos.z + 1);
    return chunks[index];
}

BlockType ChunkSnapshotM::getBlockFromLocalPos(const glm::ivec3& localPos) const {
    if (ChunkSnapshot::inCenterBounds(localPos)) {
        return center()->getBlock(localPos);
    }
    auto chunk = getChunkFromLocalPos(localPos);
    if (chunk) {
        glm::ivec3 innerLocalPos = (localPos + Chunk::CHUNK_SIZE) % Chunk::CHUNK_SIZE;
        return chunk->getBlock(innerLocalPos);
    }
    return BlockType::Air;
}

uint16_t ChunkSnapshotM::getSunLightFromLocalPos(const glm::ivec3& localPos) const {
    if (ChunkSnapshot::inCenterBounds(localPos)) {
        return center()->getSunLight(localPos);
    }
    auto chunk = getChunkFromLocalPos(localPos);
    if (chunk) {
        glm::ivec3 innerLocalPos = (localPos + Chunk::CHUNK_SIZE) % Chunk::CHUNK_SIZE;
        return chunk->getSunLight(innerLocalPos);
    }
    return 0;
}

uint16_t ChunkSnapshotM::getBlockLightFromLocalPos(const glm::ivec3& localPos) const {
    if (ChunkSnapshot::inCenterBounds(localPos)) {
        return center()->getBlockLight(localPos);
    }
    auto chunk = getChunkFromLocalPos(localPos);
    if (chunk) {
        glm::ivec3 innerLocalPos = (localPos + Chunk::CHUNK_SIZE) % Chunk::CHUNK_SIZE;
        return chunk->getBlockLight(innerLocalPos);
    }
    return 0;
}

uint16_t ChunkSnapshotM::getLightLevelFromLocalPos(const glm::ivec3& localPos) const {
    if (ChunkSnapshot::inCenterBounds(localPos)) {
        return center()->getLightLevel(localPos);
    }
    auto chunk = getChunkFromLocalPos(localPos);
    if (chunk) {
        glm::ivec3 innerLocalPos = (localPos + Chunk::CHUNK_SIZE) % Chunk::CHUNK_SIZE;
        return chunk->getLightLevel(innerLocalPos);
    }
    return 15;
}

uint16_t ChunkSnapshotM::getNearbySkyLight(const glm::ivec3 &localPos) const
{
    uint16_t maxLight = 0;
    for (int i = 0; i < 6; ++i) {
        glm::ivec3 dir = static_cast<glm::ivec3>(DirectionUtils::blockfaceDirection(static_cast<BlockFace>(i)));
        glm::ivec3 neighborPos = localPos + dir;
        if (neighborPos.x < -Chunk::CHUNK_SIZE || neighborPos.x >= Chunk::CHUNK_SIZE * 2 ||
            neighborPos.y < -Chunk::CHUNK_SIZE || neighborPos.y >= Chunk::CHUNK_SIZE * 2 ||
            neighborPos.z < -Chunk::CHUNK_SIZE || neighborPos.z >= Chunk::CHUNK_SIZE * 2) {
            continue; // Skip out of bounds neighbors
        }
        auto block = getBlockFromLocalPos(neighborPos);
        if (BlockData::isTranslucentBlock(block) || BlockData::isTransparentBlock(block)) {
            auto light = getSunLightFromLocalPos(neighborPos);
            maxLight = std::max(maxLight, light);
        }
    }
    return maxLight;
}

uint16_t ChunkSnapshotM::getNearbyBlockLight(const glm::ivec3 &localPos) const
{
    uint16_t maxLight = 0;
    for (int i = 0; i < 6; ++i) {
        glm::ivec3 dir = static_cast<glm::ivec3>(DirectionUtils::blockfaceDirection(static_cast<BlockFace>(i)));
        glm::ivec3 neighborPos = localPos + dir;
        if (neighborPos.x < -Chunk::CHUNK_SIZE || neighborPos.x >= Chunk::CHUNK_SIZE * 2 ||
            neighborPos.y < -Chunk::CHUNK_SIZE || neighborPos.y >= Chunk::CHUNK_SIZE * 2 ||
            neighborPos.z < -Chunk::CHUNK_SIZE || neighborPos.z >= Chunk::CHUNK_SIZE * 2) {
            continue; // Skip out of bounds neighbors
        }
        auto light = getBlockLightFromLocalPos(neighborPos);
        maxLight = std::max(maxLight, light);
    }
    return maxLight;
}

void ChunkSnapshotM::setBlockFromLocalPos(const glm::ivec3& localPos, BlockType type) {
    if (ChunkSnapshot::inCenterBounds(localPos)) {
        center()->setBlock(localPos, type);
    } else {
        auto chunk = getChunkFromLocalPos(localPos);
        if (chunk) {
            glm::ivec3 innerLocalPos = (localPos + Chunk::CHUNK_SIZE) % Chunk::CHUNK_SIZE;
            chunk->setBlock(innerLocalPos, type);
        }
    }
}

void ChunkSnapshotM::setSunLightFromLocalPos(const glm::ivec3& localPos, uint8_t lightLevel) {
    if (ChunkSnapshot::inCenterBounds(localPos)) {
        center()->setSunLight(localPos, lightLevel);
    } else {
        auto chunk = getChunkFromLocalPos(localPos);
        if (chunk) {
            glm::ivec3 innerLocalPos = (localPos + Chunk::CHUNK_SIZE) % Chunk::CHUNK_SIZE;
            chunk->setSunLight(innerLocalPos, lightLevel);
        }
    }
}

void ChunkSnapshotM::setBlockLightFromLocalPos(const glm::ivec3& localPos, uint8_t lightLevel) {
    if (ChunkSnapshot::inCenterBounds(localPos)) {
        center()->setBlockLight(localPos, lightLevel);
    } else {
        auto chunk = getChunkFromLocalPos(localPos);
        if (chunk) {
            glm::ivec3 innerLocalPos = (localPos + Chunk::CHUNK_SIZE) % Chunk::CHUNK_SIZE;
            chunk->setBlockLight(innerLocalPos, lightLevel);
        }
    }
}