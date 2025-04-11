#include "graphics/world_renderer.h"
#include "game_application.h"

void WorldRenderer::buildMesh(const glm::ivec3 &chunkPos)
{
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

void WorldRenderer::draw(const Camera& camera, gfx::Shader* shader, gfx::TextureAtlas<std::string>* atlas)
{
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
}