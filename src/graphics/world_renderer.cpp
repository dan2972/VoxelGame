#include "graphics/world_renderer.h"
#include "game_application.h"

WorldRenderer::WorldRenderer(ChunkMap* chunkMap, ResourceManager* resourceManager)
    : m_chunkMap(chunkMap), 
    m_resourceManager(resourceManager),
    m_chunkMapRenderer(chunkMap) 
{}

void WorldRenderer::update()
{
    m_chunkMapRenderer.updateBuildQueue(renderOptions.useSmoothLighting);
}

void WorldRenderer::loadResources()
{
    checkPointers();

    m_chunkMapRenderer.setupResources(
        m_resourceManager->getShader("chunk"),
        m_resourceManager->getTextureAtlas<BlockTexture>("chunk_atlas")
    );
}

void WorldRenderer::draw(const Camera& camera, GameWindow& window)
{
    checkPointers();
    m_chunkMapRenderer.draw(camera, renderOptions.renderDistance, renderOptions.useAO, renderOptions.aoFactor);

    if (renderOptions.showChunkBorder)
    {
        auto chunkPos = Chunk::globalToChunkPos(camera.position);
        drawChunkBorder(glm::vec3(chunkPos.x, chunkPos.y, chunkPos.z), camera, window);
    }

    if (renderOptions.showSunLightLevels || renderOptions.showBlockLightLevels)
        showLightLevels(camera);
}

void WorldRenderer::checkPointers() const
{
    if (!m_chunkMap)
        throw std::runtime_error("WorldRenderer: World pointer is null.");
    if (!m_resourceManager)
        throw std::runtime_error("WorldRenderer: ResourceManager pointer is null.");
}

void WorldRenderer::drawChunkBorder(const glm::ivec3 &chunkPos, const Camera &camera, GameWindow& window)
{
    checkPointers();
    auto lineRenderer = m_resourceManager->getLineRenderer("default");
    auto lineShader = m_resourceManager->getShader("line");

    lineRenderer->beginBatch();
    glm::ivec3 offset = glm::ivec3(Chunk::CHUNK_SIZE) / 2;
    glm::ivec3 pos = Chunk::CHUNK_SIZE * chunkPos + offset;
    glm::vec3 c1 = pos - offset;
    glm::vec3 c2 = pos + offset;
    lineRenderer->drawAA2DGrid({c1.x, c1.y, c1.z}, {c1.x, c2.y, c2.z}, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), Chunk::CHUNK_SIZE / 4, false);
    lineRenderer->drawAA2DGrid({c2.x, c1.y, c1.z}, {c2.x, c2.y, c2.z}, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), Chunk::CHUNK_SIZE / 4, false);
    lineRenderer->drawAA2DGrid({c1.x, c1.y, c1.z}, {c2.x, c1.y, c2.z}, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), Chunk::CHUNK_SIZE / 4, false);
    lineRenderer->drawAA2DGrid({c1.x, c2.y, c1.z}, {c2.x, c2.y, c2.z}, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), Chunk::CHUNK_SIZE / 4, false);
    lineRenderer->drawAA2DGrid({c1.x, c1.y, c1.z}, {c2.x, c2.y, c1.z}, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), Chunk::CHUNK_SIZE / 4, false);
    lineRenderer->drawAA2DGrid({c1.x, c1.y, c2.z}, {c2.x, c2.y, c2.z}, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), Chunk::CHUNK_SIZE / 4, false);
    lineRenderer->drawCube(pos, glm::vec3(Chunk::CHUNK_SIZE), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    
    bool isCulled = window.isEnabled(GL_CULL_FACE);
    if (isCulled)
        window.disableCulling();
    lineShader->use();
    lineShader->setMat4("uProjection", camera.getProjectionMatrix());
    lineShader->setMat4("uView", camera.getViewMatrix());
    lineShader->setMat4("uModel", glm::mat4(1.0f));
    lineShader->setFloat("uLineWidth", 1.0f);
    lineShader->setVec2("uResolution", camera.framebufferSize);
    lineRenderer->draw();
    if (isCulled)
        window.enableCulling();
}

void WorldRenderer::showLightLevels(const Camera& camera)
{
    checkPointers();
    auto fontRenderer = m_resourceManager->getFontRenderer("default_billboard");
    auto fontShader = m_resourceManager->getShader("font_billboard");

    fontShader->use();
    fontShader->setMat4("uProjection", camera.getProjectionMatrix());
    fontShader->setMat4("uView", camera.getViewMatrix());
    fontShader->setMat4("uModel", glm::mat4(1.0f));
    fontRenderer->beginBatch();

    int radius = renderOptions.showLightLevelRadius;
    glm::vec3 cameraPos = camera.position;

    int chunkRadius = radius / Chunk::CHUNK_SIZE + 1;

    auto chunks = m_chunkMap->getChunksInRadius(Chunk::globalToChunkPos(cameraPos), chunkRadius);

    for (auto chunk : chunks) {
        auto min = cameraPos - glm::vec3(radius, radius, radius);
        auto max = cameraPos + glm::vec3(radius, radius, radius);
        
        // local positions that could be out of bounds
        auto chunkOrigin = Chunk::localToGlobalPos({0,0,0}, chunk->getPos());
        auto localMin = glm::ivec3(min) - chunkOrigin;
        auto localMax = glm::ivec3(max) - chunkOrigin;
        localMin = glm::clamp(localMin, 0, Chunk::CHUNK_SIZE - 1);
        localMax = glm::clamp(localMax, 0, Chunk::CHUNK_SIZE - 1);
        auto localCam = cameraPos - glm::vec3(chunkOrigin);
        
        for (int x = localMin.x; x <= localMax.x; ++x) {
            for (int z = localMin.z; z <= localMax.z; ++z) {
                for (int y = localMin.y; y <= localMax.y; ++y) {
                    glm::vec3 d = glm::vec3(x, y, z) - localCam;
                    if ((d.x * d.x + d.y * d.y + d.z * d.z) > radius * radius)
                        continue;
                    glm::ivec3 pos = {x, y, z};

                    if (x < 0 || x >= Chunk::CHUNK_SIZE || y < 0 || y >= Chunk::CHUNK_SIZE || z < 0 || z >= Chunk::CHUNK_SIZE)
                        continue;

                    if (chunk->getBlock(pos) != BlockType::Air)
                        continue;
                    
                    auto sunLight = chunk->getSunLight(pos);
                    auto blockLight = chunk->getBlockLight(pos);

                    float sunColor = static_cast<float>(sunLight) / 15.0f;
                    float blockColor = static_cast<float>(blockLight) / 15.0f;
                    glm::vec4 sunTextColor = glm::vec4(1.0f - sunColor, sunColor, 0.2f, 1.0f);
                    glm::vec4 blockTextColor = glm::vec4(1.0f - blockColor, blockColor, 0.2f, 1.0f);
                    glm::vec3 textPos = glm::vec3(pos) + glm::vec3(chunk->getPos() * Chunk::CHUNK_SIZE) + glm::vec3(0.5f, 0.5f, 0.5f);
                    if (renderOptions.showSunLightLevels && renderOptions.showBlockLightLevels) {
                        fontRenderer->addTextBB(std::to_string(blockLight), {0,0,0}, {textPos.x, textPos.y - 0.1f, textPos.z}, 0.003f, blockTextColor, true);
                        fontRenderer->addTextBB(std::to_string(sunLight), {0,0,0}, {textPos.x, textPos.y + 0.1f, textPos.z}, 0.003f, sunTextColor, true);
                    } else {
                        if (renderOptions.showBlockLightLevels) {
                            fontRenderer->addTextBB(std::to_string(blockLight), {0,0,0}, textPos, 0.003f, blockTextColor, true);
                        } else {
                            fontRenderer->addTextBB(std::to_string(sunLight), {0,0,0}, textPos, 0.003f, sunTextColor, true);
                        }
                    }
                }
            }
        }
    }
    fontRenderer->draw();
}

void WorldRenderer::highlightVoxels(const std::vector<glm::ivec3>& voxels, const Camera &camera, GameWindow& window)
{
    checkPointers();
    auto lineRenderer = m_resourceManager->getLineRenderer("default");
    auto lineShader = m_resourceManager->getShader("line");

    lineRenderer->beginBatch();
    for (const auto& voxel : voxels) {
        glm::vec3 pos = glm::vec3(voxel.x, voxel.y, voxel.z) + glm::vec3(0.5f, 0.5f, 0.5f);
        lineRenderer->drawCube(pos, glm::vec3(1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
    }
    bool isCulled = window.isEnabled(GL_CULL_FACE);
    if (isCulled)
        window.disableCulling();
    lineShader->use();
    lineShader->setMat4("uProjection", camera.getProjectionMatrix());
    lineShader->setMat4("uView", camera.getViewMatrix());
    lineShader->setMat4("uModel", glm::mat4(1.0f));
    lineShader->setFloat("uLineWidth", 2.0f);
    lineShader->setVec2("uResolution", camera.framebufferSize);
    lineRenderer->draw();
    if (isCulled)
        window.enableCulling();
}

void WorldRenderer::showFrustum(const glm::mat4& mat, const Camera& camera, GameWindow& window)
{
    checkPointers();
    auto lineRenderer = m_resourceManager->getLineRenderer("default");
    auto lineShader = m_resourceManager->getShader("line");

    static std::array<glm::vec4, 8> ndcCorners = {
        glm::vec4{-1, -1, -1, 1}, // near bottom left
        glm::vec4{1, -1, -1, 1},  // near bottom right
        glm::vec4{1, 1, -1, 1},   // near top right
        glm::vec4{-1, 1, -1, 1},  // near top left
    
        glm::vec4{-1, -1, 1, 1},  // far bottom left
        glm::vec4{1, -1, 1, 1},   // far bottom right
        glm::vec4{1, 1, 1, 1},    // far top right
        glm::vec4{-1, 1, 1, 1},   // far top left
    };

    std::array<glm::vec3, 8> worldCorners;
    for (int i = 0; i < 8; ++i) {
        glm::vec4 world = mat * ndcCorners[i];
        world /= world.w;
        worldCorners[i] = glm::vec3(world);
    }

    auto drawLine = [&](int i, int j) {
        lineRenderer->drawLine(worldCorners[i], worldCorners[j], glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
    };

    // Draw the frustum lines
    lineRenderer->beginBatch();
    drawLine(0, 1); drawLine(1, 2); drawLine(2, 3); drawLine(3, 0); // near plane
    drawLine(4, 5); drawLine(5, 6); drawLine(6, 7); drawLine(7, 4); // far plane
    drawLine(0, 4); drawLine(1, 5); drawLine(2, 6); drawLine(3, 7); // connecting lines

    bool isCulled = window.isEnabled(GL_CULL_FACE);
    if (isCulled)
        window.disableCulling();
    lineShader->use();
    lineShader->setMat4("uProjection", camera.getProjectionMatrix());
    lineShader->setMat4("uView", camera.getViewMatrix());
    lineShader->setMat4("uModel", glm::mat4(1.0f));
    lineShader->setFloat("uLineWidth", 2.0f);
    lineShader->setVec2("uResolution", camera.framebufferSize);
    lineRenderer->draw();
    if (isCulled)
        window.enableCulling();
}