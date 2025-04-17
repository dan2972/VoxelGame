#pragma once

#include <glm/glm.hpp>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <queue>
#include "chunk_mesh.h"
#include "utils/glm_hash.h"
#include "camera.h"
#include "resource_manager.h"
#include "graphics/gfx/texture_atlas.h"
#include "graphics/gfx/shader.h"
#include "utils/blocking_queue.h"

struct ChunkReadyNode
{
    glm::ivec3 chunkPos;
    std::shared_ptr<ChunkMesh> chunkMesh;
};

class ChunkMapRenderer
{
public:
    ChunkMapRenderer() = default;
    ChunkMapRenderer(const ChunkMap* chunkMap) : m_chunkMap(chunkMap) {}
    ~ChunkMapRenderer() { stopThread(); }

    void setupResources(gfx::Shader* chunkShader, gfx::TextureAtlas<BlockTexture>* textureAtlas);

    void updateBuildQueue(bool useSmoothLighting);
    
    void queueChunkRadius(const glm::ivec3& chunkPos, int radius);
    void queueBlockUpdate(const glm::ivec3& blockPos, BlockType blockType);

    void draw(const Camera& camera, int viewDistance, bool useAO, float aoFactor);

    void meshBuildThreadFunc(const gfx::TextureAtlas<BlockTexture>& atlas, bool useSmoothLighting);

    void startBuildThread(bool useSmoothLighting);
    void stopThread() { m_stopThread = true; }

private:
    const ChunkMap* m_chunkMap = nullptr;
    std::unordered_map<glm::ivec3, std::shared_ptr<ChunkMesh>, glm_ivec3_hash, glm_ivec3_equal> m_chunkMeshes;
    std::unordered_map<glm::ivec3, std::shared_ptr<ChunkMesh>, glm_ivec3_hash, glm_ivec3_equal> m_activeChunkMeshes;
    BlockingQueue<ChunkSnapshot> m_chunksToBuild;
    BlockingQueue<ChunkReadyNode> m_chunksToSubmit;
    std::unordered_set<glm::ivec3, glm_ivec3_hash, glm_ivec3_equal> m_chunksInBuildQueue;
    std::atomic_bool m_stopThread = false;
    
    gfx::Shader* m_chunkShader = nullptr;
    gfx::TextureAtlas<BlockTexture>* m_textureAtlas = nullptr;
    
    void checkPointers() const;
    bool checkNeighborChunks(const glm::ivec3& chunkPos, bool checkSelf=false) const;
};