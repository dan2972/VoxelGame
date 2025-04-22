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
#include "utils/blocking_deque.h"
#include "utils/geometry.h"

struct ChunkReadyNode
{
    glm::ivec3 chunkPos;
    std::shared_ptr<ChunkMesh> chunkMesh;
};

class ChunkMapRenderer
{
public:
    // Chunks queued due to block updates are always added to the front of the queue
    // and may cause the queue exceed this size.
    // This value is primarily used to limit the number of chunks queued from the frustum
    // to allow for a more responsive frustum queueing.
    static const int MAX_BUILD_QUEUE_SIZE = 16;

    ChunkMapRenderer() = default;
    ChunkMapRenderer(ChunkMap* chunkMap) : m_chunkMap(chunkMap) {}
    ~ChunkMapRenderer() { stopThread(); }

    void setupResources(gfx::Shader* chunkShader, gfx::TextureAtlas<BlockTexture>* textureAtlas);

    void updateBuildQueue(bool useSmoothLighting);
    
    void queueFrustum(const Frustum& frustum, const glm::ivec3& chunkPos, int radius);
    void queueChunkRadius(const glm::ivec3& chunkPos, int radius);
    void queueBlockUpdate(const glm::ivec3& blockPos, BlockType blockType);

    void draw(const Camera& camera, int viewDistance, bool useAO, float aoFactor);

    void meshBuildThreadFunc(const gfx::TextureAtlas<BlockTexture>& atlas, bool useSmoothLighting);

    void startBuildThread(bool useSmoothLighting);
    void stopThread() { m_stopThread = true; }

private:
    ChunkMap* m_chunkMap = nullptr;
    std::unordered_map<glm::ivec3, std::shared_ptr<ChunkMesh>, glm_ivec3_hash, glm_ivec3_equal> m_chunkMeshes;
    std::unordered_map<glm::ivec3, std::shared_ptr<ChunkMesh>, glm_ivec3_hash, glm_ivec3_equal> m_activeChunkMeshes;
    BlockingDeque<ChunkSnapshot> m_chunksToBuild;
    BlockingQueue<ChunkReadyNode> m_chunksToSubmit;
    std::unordered_set<glm::ivec3, glm_ivec3_hash, glm_ivec3_equal> m_chunksInBuildQueue;
    std::atomic_bool m_stopThread = false;
    
    gfx::Shader* m_chunkShader = nullptr;
    gfx::TextureAtlas<BlockTexture>* m_textureAtlas = nullptr;
    
    void checkPointers() const;
    bool checkNeighborChunks(const glm::ivec3& chunkPos, bool checkSelf=false) const;
};