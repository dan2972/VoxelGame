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
        m_chunkMeshes.emplace(chunkPos, std::move(chunkMesh));
    }
    m_chunkMeshes.at(chunkPos)->buildMesh(*m_world, renderOptions.useSmoothLighting);
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
        shader->setVec3("uChunkOffset", glm::vec3(chunkPos.x, chunkPos.y, chunkPos.z));
        chunkMesh->draw();
    }

    if (renderOptions.showChunkBorder)
    {
        auto chunkPos = Chunk::globalToChunkPos(camera.position);
        drawChunkBorder(glm::vec3(chunkPos.x, chunkPos.y, chunkPos.z), camera);
    }
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