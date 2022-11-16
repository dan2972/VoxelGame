#include "chunk.h"

Chunk::Chunk() {
	m_chunk.fill(Grass);
}

BlockType Chunk::getBlockAt(unsigned x, unsigned y, unsigned z) const {
	return m_chunk[y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x];
}

void Chunk::removeBlock(unsigned x, unsigned y, unsigned z) {
	m_chunk[y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x] = Air;
}

void Chunk::placeBlock(unsigned x, unsigned y, unsigned z, BlockType type) {
	if(m_chunk[y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x] == Air)
		m_chunk[y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x] = type;
}