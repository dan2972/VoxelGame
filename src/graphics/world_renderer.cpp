#include "graphics/world_renderer.h"
#include "game_application.h"

void WorldRenderer::loadResources(const World* world, ResourceManager* resourceManager)
{
    m_world = world;
    m_resourceManager = resourceManager;

    checkPointers();

    m_resourceManager->loadShader("chunk", "res/shaders/terrain_chunk.vert", "res/shaders/terrain_chunk.frag");
    auto atlas = m_resourceManager->addTextureAtlas("chunk_atlas", {.internalFilter = GL_NEAREST});
    atlas->addImgFromPath("grass", "res/textures/grass.png");
    atlas->addImgFromPath("dirt", "res/textures/dirt.png");
    atlas->addImgFromPath("stone", "res/textures/stone.png");
    atlas->addImgFromPath("wood_planks", "res/textures/wood_planks.png");
}

void WorldRenderer::buildMesh(const glm::ivec3 &chunkPos)
{
    checkPointers();
    
    Chunk* chunk = m_world->getChunk(chunkPos);
    if (!chunk)
        return;
    auto it = m_chunkMeshes.find(chunkPos);
    if (it == m_chunkMeshes.end())
    {
        auto chunkMesh = std::make_unique<ChunkMesh>(chunk);
        chunkMesh->setup();
        auto pair = m_chunkMeshes.emplace(chunkPos, std::move(chunkMesh));
        it = pair.first;
    }
    it->second->buildMesh(*m_world, renderOptions.useSmoothLighting);
}

void WorldRenderer::draw(const Camera& camera)
{
    checkPointers();
    auto shader = m_resourceManager->getShader("chunk");
    auto atlas = m_resourceManager->getTextureAtlas("chunk_atlas");
    atlas->use();
    shader->use();
    shader->setMat4("uProjection", camera.getProjectionMatrix());
    shader->setMat4("uView", camera.getViewMatrix());
    shader->setMat4("uModel", glm::mat4(1.0f));
    shader->setBool("uAOEnabled", renderOptions.useAO);
    shader->setFloat("uAOIntensity", renderOptions.aoFactor);

    for (auto& [chunkPos, chunkMesh] : m_chunkMeshes)
    {
        shader->setVec3("uChunkOffset", glm::vec3(chunkPos) * float(Chunk::CHUNK_SIZE));
        chunkMesh->draw();
    }

    if (renderOptions.showChunkBorder)
    {
        auto chunkPos = Chunk::globalToChunkPos(camera.position);
        drawChunkBorder(glm::vec3(chunkPos.x, chunkPos.y, chunkPos.z), camera);
    }

    if (renderOptions.showSunLightLevels || renderOptions.showBlockLightLevels)
        showLightLevels(camera);
}

void WorldRenderer::checkPointers() const
{
    if (!m_world)
        throw std::runtime_error("WorldRenderer: World pointer is null.");
    if (!m_resourceManager)
        throw std::runtime_error("WorldRenderer: ResourceManager pointer is null.");
}

void WorldRenderer::drawChunkBorder(const glm::ivec3 &chunkPos, const Camera &camera)
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
    lineRenderer->drawCube(pos, glm::vec3(Chunk::CHUNK_SIZE)*0.999f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    lineShader->use();
    lineShader->setMat4("uProjection", camera.getProjectionMatrix());
    lineShader->setMat4("uView", camera.getViewMatrix());
    lineShader->setMat4("uModel", glm::mat4(1.0f));
    lineShader->setFloat("uLineWidth", 1.0f);
    lineShader->setVec2("uResolution", camera.resolution);
    lineRenderer->draw();
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

    auto chunks = m_world->getChunksInRadius(Chunk::globalToChunkPos(cameraPos), chunkRadius);

    for (Chunk* chunk : chunks) {
        auto min = cameraPos - glm::vec3(radius, radius, radius);
        auto max = cameraPos + glm::vec3(radius, radius, radius);
        
        // local positions that could be out of bounds
        auto localMin = glm::ivec3(min) - Chunk::localToGlobalPos({0,0,0}, chunk->getPos());
        auto localMax = glm::ivec3(max) - Chunk::localToGlobalPos({0,0,0}, chunk->getPos());
        localMin = glm::clamp(localMin, 0, Chunk::CHUNK_SIZE - 1);
        localMax = glm::clamp(localMax, 0, Chunk::CHUNK_SIZE - 1);
        auto localCam = cameraPos - glm::vec3(Chunk::localToGlobalPos({0,0,0}, chunk->getPos()));
        
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