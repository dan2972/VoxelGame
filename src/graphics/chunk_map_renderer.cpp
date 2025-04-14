#include "graphics/chunk_map_renderer.h"

void ChunkMapRenderer::setupResources(gfx::Shader* chunkShader, gfx::TextureAtlas<std::string>* textureAtlas) 
{
    m_chunkShader = chunkShader;
    m_textureAtlas = textureAtlas;
}

void ChunkMapRenderer::updateBuildQueue(bool useSmoothLighting) 
{
    checkPointers();
    int meshBuildCount = 0;
    while (!m_chunkUpdateQueue.empty())
    {
        auto chunkPos = m_chunkUpdateQueue.top().chunkPos;
        m_chunkUpdateQueue.pop();
        if (meshBuildCount < 1 && buildMesh(chunkPos, useSmoothLighting)) {
            meshBuildCount++;
        }
    }
}

void ChunkMapRenderer::queueChunkRadius(const glm::ivec3& chunkPos, int radius) 
{
    for (int x = -radius; x <= radius; ++x)
    {
        for (int y = -radius; y <= radius; ++y)
        {
            for (int z = -radius; z <= radius; ++z)
            {
                int dx = x - chunkPos.x;
                int dy = y - chunkPos.y;
                int dz = z - chunkPos.z;
                int distance = dx * dx + dy * dy + dz * dz;
                glm::ivec3 pos = chunkPos + glm::ivec3(x, y, z);
                if (!m_chunkMeshes.contains(pos))
                {
                    m_chunkUpdateQueue.push(ChunkQueueNode{pos, distance});
                }
            }
        }
    }
}

bool ChunkMapRenderer::buildMesh(const glm::ivec3 &chunkPos, bool useSmoothLighting)
{
    checkPointers();
    
    Chunk* chunk = m_chunkMap->getChunk(chunkPos);
    if (!chunk)
        return false;
    if (!checkNeighborChunks(chunkPos))
        return false;
    auto it = m_chunkMeshes.find(chunkPos);
    if (it == m_chunkMeshes.end())
    {
        auto chunkMesh = std::make_unique<ChunkMesh>(chunk);
        chunkMesh->setup();
        auto pair = m_chunkMeshes.emplace(chunkPos, std::move(chunkMesh));
        it = pair.first;
    }
    it->second->buildMesh(*m_chunkMap, useSmoothLighting);
    return true;
}

void ChunkMapRenderer::draw(const Camera& camera, bool useAO, float aoFactor)
{
    checkPointers();
    m_textureAtlas->use();
    m_chunkShader->use();
    m_chunkShader->setMat4("uProjection", camera.getProjectionMatrix());
    m_chunkShader->setMat4("uView", camera.getViewMatrix());
    m_chunkShader->setMat4("uModel", glm::mat4(1.0f));
    m_chunkShader->setBool("uAOEnabled", useAO);
    m_chunkShader->setFloat("uAOIntensity", aoFactor);

    for (auto& [chunkPos, chunkMesh] : m_chunkMeshes)
    {
        m_chunkShader->setVec3("uChunkOffset", glm::vec3(chunkPos) * float(Chunk::CHUNK_SIZE));
        chunkMesh->draw();
    }
}

bool ChunkMapRenderer::checkNeighborChunks(const glm::ivec3& chunkPos, bool checkSelf) const
{
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            for (int z = -1; z <= 1; ++z)
            {
                if (x == 0 && y == 0 && z == 0 && !checkSelf)
                    continue;
                glm::ivec3 neighborPos = chunkPos + glm::ivec3(x, y, z);
                if (!m_chunkMap->getChunk(neighborPos))
                    return false;
            }
        }
    }
    return true;
}

void ChunkMapRenderer::checkPointers() const
{
    if (!m_chunkShader)
        throw std::runtime_error("ChunkMapRenderer: Shader pointer is null.");
    if (!m_textureAtlas)
        throw std::runtime_error("ChunkMapRenderer: TextureAtlas pointer is null.");
}