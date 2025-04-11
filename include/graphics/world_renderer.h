#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <memory>
#include "chunk_mesh.h"
#include "utils/glm_hash.h"
#include "camera.h"
#include "world/world.h"
#include "graphics/gfx/shader.h"
#include "graphics/gfx/texture_atlas.h"
#include "resource_manager.h"

struct RenderOptions
{
    bool useAO = true;
    bool useSmoothLighting = true;
    bool showChunkBorder = false;
    float aoFactor = 0.5f;
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

    void loadResources(const World* world, ResourceManager* resourceManager);

    void buildMesh(const glm::ivec3& chunkPos);

    void draw(const Camera& camera);
private:
    const World* m_world = nullptr;
    ResourceManager* m_resourceManager = nullptr;
    std::unordered_map<glm::ivec3, std::unique_ptr<ChunkMesh>, glm_ivec3_hash, glm_ivec3_equal> m_chunkMeshes;

    void checkPointers() const;
    void drawChunkBorder(const glm::ivec3& chunkPos, const Camera& camera);
};