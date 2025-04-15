#include "world/chunk_snapshot.h"

ChunkSnapshot::ChunkSnapshot() {
    std::fill(chunks.begin(), chunks.end(), nullptr);
}

ChunkSnapshot::ChunkSnapshot(const std::array<std::shared_ptr<const Chunk>, 27>& chunks) : chunks(chunks) {}

ChunkSnapshot::ChunkSnapshot(const ChunkMap& chunkMap, const glm::ivec3& centerChunkPos) {
    auto centerChunk = chunkMap.getChunk(centerChunkPos);
    if (centerChunk) {
        chunks[13] = centerChunk;
        for (const auto& dir : getRequiredChunkDirs()) {
            auto chunk = chunkMap.getChunk(centerChunkPos + dir);
            if (chunk) {
                chunks[(dir.x + 1) * 9 + (dir.y + 1) * 3 + (dir.z + 1)] = chunk;
            }
        }
    }
}

bool ChunkSnapshot::CreateSnapshot(const ChunkMap& chunkMap, const glm::ivec3& centerChunkPos, ChunkSnapshot& snapshot) {
    auto centerChunk = chunkMap.getChunk(centerChunkPos);
    if (!centerChunk)
        return false;

    snapshot.chunks[13] = centerChunk;
    for (const auto& dir : getRequiredChunkDirs()) {
        auto chunk = chunkMap.getChunk(centerChunkPos + dir);
        if (!chunk) 
            return false;
        snapshot.chunks[(dir.x + 1) * 9 + (dir.y + 1) * 3 + (dir.z + 1)] = chunk;
    }
    return true;
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

bool ChunkSnapshot::isValid() const {
    return std::all_of(chunks.begin(), chunks.end(), [](const auto& chunk) { return chunk != nullptr; });
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