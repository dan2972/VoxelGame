#pragma once

#include <glm/glm.hpp>
#include "camera.h"
#include "world/chunk_map.h"
#include "graphics/chunk_map_renderer.h"
#include "graphics/world_render_options.h"

class WorldRenderer
{
public:
    RenderOptions renderOptions;

    WorldRenderer() = default;
    WorldRenderer(const ChunkMap* chunkMap, ResourceManager* resourceManager);
    ~WorldRenderer() = default;

    WorldRenderer(const WorldRenderer&) = delete;
    WorldRenderer& operator=(const WorldRenderer&) = delete;
    WorldRenderer(WorldRenderer&&) = default;
    WorldRenderer& operator=(WorldRenderer&&) = default;

    void update();

    void loadResources();

    void draw(const Camera& camera);

    ChunkMapRenderer& getChunkMapRenderer() { return m_chunkMapRenderer; }
private:
    const ChunkMap* m_chunkMap = nullptr;
    ResourceManager* m_resourceManager = nullptr;
    ChunkMapRenderer m_chunkMapRenderer;

    void checkPointers() const;
    void drawChunkBorder(const glm::ivec3& chunkPos, const Camera& camera);
    void showLightLevels(const Camera& camera);
};