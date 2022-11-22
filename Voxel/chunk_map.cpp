#include "chunk_map.h"
#include <iostream>

void ChunkMap::addChunk(Chunk* chunk) {
	m_chunkMap.emplace(ChunkCoord{ chunk->getChunkX(), chunk->getChunkZ() }, std::unique_ptr<Chunk>(chunk));
}

Chunk* ChunkMap::getChunk(int chunkX, int chunkY) const {
	auto it = m_chunkMap.find(ChunkCoord{ chunkX, chunkY });
	if (it != m_chunkMap.end()) {
		return it->second.get();
	}
	return nullptr;
}

BlockType ChunkMap::getBlockAt(int x, int y, int z) const {
	int chunkX = x >= 0 ? x / Chunk::CHUNK_SIZE : (x - Chunk::CHUNK_SIZE + 1) / Chunk::CHUNK_SIZE;
	int chunkZ = z >= 0 ? z / Chunk::CHUNK_SIZE : (z - Chunk::CHUNK_SIZE + 1) / Chunk::CHUNK_SIZE;
	int localX = x - chunkX * Chunk::CHUNK_SIZE;
	int localZ = z - chunkZ * Chunk::CHUNK_SIZE;
	Chunk* chunk = getChunk(chunkX, chunkZ);
	if (chunk != nullptr) {
		return chunk->getBlockAt(localX, y, localZ);
	}
	return Air;
}