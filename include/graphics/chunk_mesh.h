#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <atomic>
#include "world/chunk.h"
#include "world/world.h"
#include "graphics/gfx/mesh.h"
#include "utils/direction_utils.h"
#include "world/chunk_snapshot.h"
#include "world/block_data.h"
#include "graphics/gfx/texture_atlas.h"

enum class RenderLayer
{
    Opaque = 0,
    Translucent = 1,
    Transparent = 2
};

class ChunkMesh
{
public:
    ChunkMesh() = default;
    ~ChunkMesh() = default;

    ChunkMesh(const ChunkMesh&) = delete;
    ChunkMesh& operator=(const ChunkMesh&) = delete;
    ChunkMesh(ChunkMesh&&) = default;
    ChunkMesh& operator=(ChunkMesh&&) = default;

    void setup();

    void draw(RenderLayer layer=RenderLayer::Opaque);

    void clearMesh();

    void buildMesh(const ChunkSnapshot& snapshot, const gfx::TextureAtlas<BlockTexture>& atlas, bool smoothLighting=true);

    void submitBuffers();

    void setDirty(bool dirty) { m_dirty.store(dirty); }
    bool isDirty() const { return m_dirty.load(); }
    
private:
    gfx::Mesh m_mesh;
    std::vector<float> m_vertices;
    std::vector<unsigned int> m_indices;
    unsigned int m_indexCounter = 0;

    gfx::Mesh m_meshTranslucent;
    std::vector<float> m_verticesTranslucent;
    std::vector<unsigned int> m_indicesTranslucent;
    unsigned int m_indexCounterTranslucent = 0;

    gfx::Mesh m_meshTransparent;
    std::vector<float> m_verticesTransparent;
    std::vector<unsigned int> m_indicesTransparent;
    unsigned int m_indexCounterTransparent = 0;

    std::atomic<bool> m_dirty = false;

    void addFace(
        const glm::ivec3 &pos, 
        BlockFace face, 
        std::array<float, 8> texCoords, 
        std::array<int, 4> aoValues, 
        std::array<glm::vec4, 4> lightLevels, 
        RenderLayer layer,
        bool flipQuad
    );

    std::array<int, 12> getFaceCoords(BlockFace face);
    std::array<int, 4> getAOValues(const glm::ivec3& blockPos, BlockFace face, const ChunkSnapshot& snapshot);
    std::array<glm::vec4, 4> getLightValues(const glm::ivec3& blockPos, BlockFace face, const ChunkSnapshot& snapshot, bool smoothLighting=true);
    bool shouldFlipQuad(std::array<int, 4> aoValues);

    void getAOBlockPos(const glm::ivec3& cornerPos, BlockFace face, glm::ivec3* outs1, glm::ivec3* outs2, glm::ivec3* outc);
    static int vertexAO(bool side1, bool side2, bool corner);
    bool shouldRenderFace(BlockType curBlock, BlockType neighbor);

    RenderLayer getRenderLayer(BlockType blockType);
};