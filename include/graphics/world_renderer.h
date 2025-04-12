#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <memory>
#include <queue>
#include "chunk_mesh.h"
#include "utils/glm_hash.h"
#include "camera.h"
#include "world/world.h"
#include "graphics/gfx/shader.h"
#include "graphics/gfx/texture_atlas.h"
#include "resource_manager.h"
#include "world/chunkQueueNode.h"

struct RenderOptions
{
    bool useAO = true;
    bool useSmoothLighting = true;
    bool showChunkBorder = false;
    bool showSunLightLevels = false;
    bool showBlockLightLevels = false;
    float showLightLevelRadius = Chunk::CHUNK_SIZE / 2;
    float aoFactor = 0.5f;
    int chunkDequeueSize = 1;
};

class WorldRenderer
{
public:
    RenderOptions renderOptions;

    WorldRenderer() = default;
    ~WorldRenderer() = default;

    WorldRenderer(const WorldRenderer&) = delete;
    WorldRenderer& operator=(const WorldRenderer&) = delete;
    WorldRenderer(WorldRenderer&&) = default;
    WorldRenderer& operator=(WorldRenderer&&) = default;

    void update();

    void queueChunkRadius(const glm::ivec3& chunkPos, int radius);

    void loadResources(const World* world, ResourceManager* resourceManager);

    bool buildMesh(const glm::ivec3& chunkPos);

    void draw(const Camera& camera);

    bool checkNeighborChunks(const glm::ivec3& chunkPos, bool checkSelf=false) const;
private:
    const World* m_world = nullptr;
    ResourceManager* m_resourceManager = nullptr;
    std::unordered_map<glm::ivec3, std::unique_ptr<ChunkMesh>, glm_ivec3_hash, glm_ivec3_equal> m_chunkMeshes;
    std::priority_queue<ChunkQueueNode> m_chunkUpdateQueue;

    void checkPointers() const;
    void drawChunkBorder(const glm::ivec3& chunkPos, const Camera& camera);
    void showLightLevels(const Camera& camera);
};