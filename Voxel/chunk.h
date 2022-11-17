#pragma once
#include <array>
enum BlockType {
	Air,
	Grass
};

class Chunk
{
public:
	static const unsigned CHUNK_SIZE = 11;
	Chunk();

	BlockType getBlockAt(unsigned x, unsigned y, unsigned z) const;
	void placeBlock(unsigned x, unsigned y, unsigned z, BlockType type);
	void removeBlock(unsigned x, unsigned y, unsigned z);

private:
	std::array<BlockType, CHUNK_SIZE* CHUNK_SIZE* CHUNK_SIZE> m_chunk{};
};

