#pragma once
#include <unordered_map>
#include <memory>
#include "chunk.h"
class ChunkMap
{
public:
	ChunkMap() = default;

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

    void addChunk(Chunk* chunk);

    // returns a chunk at the given chunk coordinates
    // returns a nullptr if the chunk does not exist
    Chunk* getChunk(int chunkX, int chunkY) const;

    // returns a block type at the given map coordinates
    BlockType getBlockAt(int x, int y, int z) const;
private:
	std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>, HashFunc, EqualsFunc> m_chunkMap;
};

