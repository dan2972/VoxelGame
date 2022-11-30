#pragma once
#include <unordered_map>
#include "chunk_map.h"
#include "chunk.h"
#include "mesh.h"
class ChunkRenderer
{
public:
	ChunkRenderer(const ChunkMap& chunkmap);

    void drawChunk(int chunkX, int chunkZ);
private:
	const ChunkMap* m_chunkMap = nullptr;
	std::unordered_map<ChunkMap::ChunkCoord, std::unique_ptr<Mesh>, ChunkMap::HashFunc, ChunkMap::EqualsFunc> m_meshMap;

    void generateMesh(const Chunk& chunk, Mesh& mesh);
    void generateLightMap(Chunk& chunk);
    bool shouldRenderLeft(const Chunk& chunk, int x, int y, int z);
    bool shouldRenderRight(const Chunk& chunk, int x, int y, int z);
    bool shouldRenderFront(const Chunk& chunk, int x, int y, int z);
    bool shouldRenderBack(const Chunk& chunk, int x, int y, int z);
    bool shouldRenderUp(const Chunk& chunk, int x, int y, int z);
    bool shouldRenderDown(const Chunk& chunk, int x, int y, int z);

    unsigned m_sunlightLevel = 15;
};

