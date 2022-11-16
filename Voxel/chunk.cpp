#include "chunk.h"
#include <random>

Chunk::Chunk() {
	//m_chunk.fill(Grass);
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist6(0, 1);
	for (unsigned i = 0; i < m_chunk.size(); ++i) {
		if (dist6(rng)) {
			m_chunk[i] = Grass;
		}
	}
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