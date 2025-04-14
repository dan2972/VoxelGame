#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <memory>
#include <queue>
#include "chunk_mesh.h"
#include "utils/glm_hash.h"
#include "camera.h"
#include "resource_manager.h"
#include "graphics/gfx/texture_atlas.h"
#include "graphics/gfx/shader.h"

class ChunkMapRenderer
{
public:
    ChunkMapRenderer() = default;
    ChunkMapRenderer(const ChunkMap* chunkMap) : m_chunkMap(chunkMap) {}
    ~ChunkMapRenderer() = default;

    void setupResources(gfx::Shader* chunkShader, gfx::TextureAtlas<std::string>* textureAtlas);

    void updateBuildQueue(bool useSmoothLighting);
    
    void queueChunkRadius(const glm::ivec3& chunkPos, int radius);

    bool buildMesh(const glm::ivec3& chunkPos, bool useSmoothLighting);

    void draw(const Camera& camera, bool useAO, float aoFactor);

private:
    const ChunkMap* m_chunkMap = nullptr;
    std::unordered_map<glm::ivec3, std::unique_ptr<ChunkMesh>, glm_ivec3_hash, glm_ivec3_equal> m_chunkMeshes;
    std::priority_queue<ChunkQueueNode> m_chunkUpdateQueue;
    
    gfx::Shader* m_chunkShader = nullptr;
    gfx::TextureAtlas<std::string>* m_textureAtlas = nullptr;
    
    void checkPointers() const;
    bool checkNeighborChunks(const glm::ivec3& chunkPos, bool checkSelf=false) const;
};