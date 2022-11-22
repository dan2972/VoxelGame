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
    struct ChunkCoord {
        ChunkCoord(int x, int z) : x{ x }, z{ z } {}
        int x, z;
    };

    struct HashFunc {
        size_t operator()(const ChunkCoord& c) const {
            size_t h1 = std::hash<int>()(c.x);
            size_t h2 = std::hash<int>()(c.z);
            return (h1 ^ (h2 << 1));
        }
    };

    struct EqualsFunc {
        bool operator()(const ChunkCoord& lhs, const ChunkCoord& rhs) const {
            return (lhs.x == rhs.x) && (lhs.z == rhs.z);
        }
    };

	const ChunkMap* m_chunkMap = nullptr;
	std::unordered_map<ChunkCoord, std::unique_ptr<Mesh>, HashFunc, EqualsFunc> m_meshMap;

    void generateMesh(const Chunk& chunk, Mesh& mesh);
};

