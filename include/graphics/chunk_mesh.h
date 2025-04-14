#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "world/chunk.h"
#include "world/world.h"
#include "graphics/gfx/mesh.h"
#include "utils/direction_utils.h"

class ChunkMesh
{
public:
    bool waitingGeneration = false;

    ChunkMesh() = default;
    ChunkMesh(const Chunk* chunk);
    ~ChunkMesh() = default;

    ChunkMesh(const ChunkMesh&) = delete;
    ChunkMesh& operator=(const ChunkMesh&) = delete;
    ChunkMesh(ChunkMesh&&) = default;
    ChunkMesh& operator=(ChunkMesh&&) = default;

    void setup();

    void draw();

    void clearMesh();

    void buildMesh(const ChunkMap& chunkMap, bool smoothLighting=true);

    void submitBuffers();

private:
    const Chunk* m_chunk = nullptr;
    gfx::Mesh m_mesh;
    std::vector<float> m_vertices;
    std::vector<unsigned int> m_indices;
    unsigned int m_indexCounter = 0;

    void addFace(
        const glm::ivec3 &pos, 
        BlockFace face, 
        std::array<float, 8> texCoords, 
        std::array<int, 4> aoValues, 
        std::array<float, 4> lightLevels, 
        bool flipQuad
    );

    std::array<int, 12> getFaceCoords(BlockFace face);
    std::array<int, 4> getAOValues(const glm::ivec3& blockPos, BlockFace face, const ChunkMap& chunkMap);
    std::array<float, 4> getLightValues(const glm::ivec3& blockPos, BlockFace face, const ChunkMap& chunkMap, bool smoothLighting=true);
    bool shouldFlipQuad(std::array<int, 4> aoValues);

    void getAOBlockPos(const glm::ivec3& cornerPos, BlockFace face, glm::ivec3* outs1, glm::ivec3* outs2, glm::ivec3* outc);
    static int vertexAO(bool side1, bool side2, bool corner);
};