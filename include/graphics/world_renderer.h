#pragma once

#include <glm/glm.hpp>
#include "camera.h"
#include "world/chunk_map.h"
#include "graphics/chunk_map_renderer.h"
#include "graphics/world_render_options.h"
#include "game_window.h"

class WorldRenderer
{
public:
    RenderOptions renderOptions;

    WorldRenderer() = default;
    WorldRenderer(const ChunkMap* chunkMap, ResourceManager* resourceManager);
    ~WorldRenderer() = default;

    WorldRenderer(const WorldRenderer&) = delete;
    WorldRenderer& operator=(const WorldRenderer&) = delete;

    void update();

    void loadResources();

    void draw(const Camera& camera, GameWindow& window);

    void highlightVoxels(const std::vector<glm::ivec3>& voxels, const Camera &camera, GameWindow& window);
    void showViewFrustum(const glm::mat4& mat, const Camera& camera, GameWindow& window);

    ChunkMapRenderer& getChunkMapRenderer() { return m_chunkMapRenderer; }
private:
    const ChunkMap* m_chunkMap = nullptr;
    ResourceManager* m_resourceManager = nullptr;
    ChunkMapRenderer m_chunkMapRenderer;

    void checkPointers() const;
    void drawChunkBorder(const glm::ivec3& chunkPos, const Camera& camera, GameWindow& window);
    void showLightLevels(const Camera& camera);
};