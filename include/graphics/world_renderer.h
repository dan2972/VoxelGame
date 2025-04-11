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

struct RenderOptions
{
    bool useAO = true;
    bool useSmoothLighting = true;
    float aoFactor = 0.5f;
};

class WorldRenderer
{
public:
    RenderOptions renderOptions;
    
    WorldRenderer() = default;
    WorldRenderer(const World* world) : m_world(world) {}
    ~WorldRenderer() = default;

    WorldRenderer(const WorldRenderer&) = delete;
    WorldRenderer& operator=(const WorldRenderer&) = delete;
    WorldRenderer(WorldRenderer&&) = default;
    WorldRenderer& operator=(WorldRenderer&&) = default;

    void buildMesh(const glm::ivec3& chunkPos);

    void draw(const Camera& camera, gfx::Shader* shader, gfx::TextureAtlas<std::string>* atlas);
private:
    const World* m_world = nullptr;
    std::unordered_map<glm::ivec3, std::unique_ptr<ChunkMesh>, glm_ivec3_hash, glm_ivec3_equal> m_chunkMeshes;
};