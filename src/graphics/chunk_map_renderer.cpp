#include "graphics/chunk_map_renderer.h"

void ChunkMapRenderer::setupResources(gfx::Shader* chunkShader, gfx::TextureAtlas<BlockType>* textureAtlas) 
{
    m_chunkShader = chunkShader;
    m_textureAtlas = textureAtlas;
}

void ChunkMapRenderer::updateBuildQueue(bool useSmoothLighting) 
{
    checkPointers();

    int meshSubmitCount = 0;
    while(!m_chunksToSubmit.empty())
    {
        ChunkReadyNode node;
        m_chunksToSubmit.pop(node);
        node.chunkMesh->setup();
        // node.chunkMesh->submitBuffers();

        m_chunkMeshes[node.chunkPos] = node.chunkMesh;
        m_chunksInBuildQueue.erase(node.chunkPos);
        meshSubmitCount++;
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
                glm::ivec3 pos = chunkPos + glm::ivec3(x, y, z);
                if (m_chunkMeshes.contains(pos))
                    continue;
                if (m_chunksInBuildQueue.contains(pos))
                    continue;
                
                ChunkSnapshot snapshot;
                if (!ChunkSnapshot::CreateSnapshot(*m_chunkMap, pos, snapshot))
                    continue;
                m_chunksInBuildQueue.insert(pos);
                m_chunksToBuild.push(snapshot);
            }
        }
    }
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

void ChunkMapRenderer::meshBuildThreadFunc(const gfx::TextureAtlas<BlockType>& atlas, bool useSmoothLighting) {
    while (true)
    {
        if (m_stopThread)
            break;
        ChunkSnapshot snapshot;
        m_chunksToBuild.pop(snapshot);
        auto chunkMesh = std::make_shared<ChunkMesh>();
        chunkMesh->buildMesh(snapshot, atlas, useSmoothLighting);
        m_chunksToSubmit.push({snapshot.center()->getPos(), chunkMesh});
    }
}

void ChunkMapRenderer::startBuildThread(bool useSmoothLighting) {
    m_stopThread = false;
    std::thread([this, useSmoothLighting]() { meshBuildThreadFunc(*m_textureAtlas, useSmoothLighting); }).detach();
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