#include "chunk.h"
#include <random>
#include "perlin_generator.h"

Chunk::Chunk(int chunkX, int chunkZ) : m_chunkX{ chunkX }, m_chunkZ{ chunkZ } {
	m_chunk.fill(Air);
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist6(0, 1);
	/*
	for (unsigned i = 0; i < m_chunk.size(); ++i) {
		if (dist6(rng)) {
			m_chunk[i] = Grass;
		}
	}*/
	for (unsigned i = 0; i < CHUNK_SIZE; ++i) {
		for (unsigned j = 0; j < CHUNK_SIZE; ++j) {
			float p = 1.0f - PerlinGenerator::getValueAt(chunkX * CHUNK_SIZE + j, chunkZ * CHUNK_SIZE + i, 0.02, 6);
			for (unsigned y = 0; y < CHUNK_SIZE * p; ++y) {
				placeBlock(j, y, i, Grass);
			}
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

int Chunk::getChunkX() const {
	return m_chunkX;
}

int Chunk::getChunkZ() const {
	return m_chunkZ;
}

int Chunk::getBlockLight(unsigned x, unsigned y, unsigned z) const
{
	return m_lightMap[y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x] & 0xF;
}

int Chunk::getSunLight(unsigned x, unsigned y, unsigned z) const
{
	return (m_lightMap[y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x] >> 4) & 0xF;
}

void Chunk::setBlockLight(unsigned x, unsigned y, unsigned z, int val)
{
	m_lightMap[y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x] = (m_lightMap[y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x] & 0xF0) | val;
}

void Chunk::setSunLight(unsigned x, unsigned y, unsigned z, int val)
{
	m_lightMap[y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x] = (m_lightMap[y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x] & 0xF) | (val << 4);
}
