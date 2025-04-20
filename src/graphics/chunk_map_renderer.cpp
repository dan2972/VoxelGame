#include "graphics/chunk_map_renderer.h"
#include "utils/algorithms.h"

void ChunkMapRenderer::setupResources(gfx::Shader* chunkShader, gfx::TextureAtlas<BlockTexture>* textureAtlas) 
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
        m_activeChunkMeshes[node.chunkPos] = node.chunkMesh;
        m_chunksInBuildQueue.erase(node.chunkPos);
        meshSubmitCount++;
    }

}

void ChunkMapRenderer::queueChunkRadius(const glm::ivec3& chunkPos, int radius) 
{
    std::vector<glm::ivec3> chunksToAdd = algo::getPosFromCenter(chunkPos, radius);
    for (const auto& pos : chunksToAdd)
    {
        if (m_chunkMeshes.contains(pos)) {
            m_activeChunkMeshes[pos] = m_chunkMeshes[pos];
            continue;
        }
        if (m_chunksInBuildQueue.contains(pos))
            continue;
        
        ChunkSnapshot snapshot;
        if (!ChunkSnapshot::CreateSnapshot(*m_chunkMap, pos, snapshot))
            continue;
        m_chunksInBuildQueue.insert(pos);
        m_chunksToBuild.pushBack(snapshot);
    }
}

void ChunkMapRenderer::queueBlockUpdate(const glm::ivec3& blockPos, BlockType blockType)
{
    glm::ivec3 chunkPos = Chunk::globalToChunkPos(blockPos);
    ChunkSnapshot snapshot;

    // if the block type is air (removing a block), build the boundaries first
    bool buildCenterChunkFirst = blockType != BlockType::Air;
    if (!buildCenterChunkFirst) {
        if (ChunkSnapshot::CreateSnapshot(*m_chunkMap, chunkPos, snapshot)) {
            m_chunksToBuild.pushFront(snapshot);
            m_chunksInBuildQueue.insert(chunkPos);
        }
    }

    glm::ivec3 localPos = Chunk::globalToLocalPos(blockPos);
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dz = -1; dz <= 1; ++dz) {
                if (dx == 0 && dy == 0 && dz == 0) continue;
    
                glm::ivec3 offset = {dx, dy, dz};
                // The boundary value we have to check
                // -1 means no boundary check, 0 means check the first element, and Chunk::CHUNK_SIZE - 1 means check the last element.
                glm::ivec3 boundary = {
                    dx < 0 ? 0 : (dx > 0 ? Chunk::CHUNK_SIZE - 1 : -1),
                    dy < 0 ? 0 : (dy > 0 ? Chunk::CHUNK_SIZE - 1 : -1),
                    dz < 0 ? 0 : (dz > 0 ? Chunk::CHUNK_SIZE - 1 : -1)
                };
    
                if ((boundary.x == -1 || localPos.x == boundary.x) &&
                    (boundary.y == -1 || localPos.y == boundary.y) &&
                    (boundary.z == -1 || localPos.z == boundary.z)) {
    
                    glm::ivec3 neighborChunkPos = chunkPos + offset;
                    ChunkSnapshot snapshotAdj;
                    if (ChunkSnapshot::CreateSnapshot(*m_chunkMap, neighborChunkPos, snapshotAdj)) {
                        m_chunksToBuild.pushFront(snapshotAdj);
                        m_chunksInBuildQueue.insert(neighborChunkPos);
                    }
                }
            }
        }
    }

    if (buildCenterChunkFirst) {
        if (ChunkSnapshot::CreateSnapshot(*m_chunkMap, chunkPos, snapshot)) {
            m_chunksToBuild.pushFront(snapshot);
            m_chunksInBuildQueue.insert(chunkPos);
        }
    }
}

void ChunkMapRenderer::draw(const Camera& camera, int viewDistance, bool useAO, float aoFactor)
{
    checkPointers();

    glm::ivec3 cameraChunkPos = Chunk::globalToChunkPos(camera.position);

    std::queue<glm::ivec3> chunksToUnLoad;
    for (auto& [chunkPos, chunkMesh] : m_activeChunkMeshes)
    {
        glm::ivec3 delta = chunkPos - cameraChunkPos;
        if (abs(delta.x) > viewDistance || abs(delta.y) > viewDistance || abs(delta.z) > viewDistance)
        {
            chunksToUnLoad.push(chunkPos);
        }
    }
    while (!chunksToUnLoad.empty())
    {
        glm::ivec3 chunkPos = chunksToUnLoad.front();
        chunksToUnLoad.pop();
        m_activeChunkMeshes.erase(chunkPos);
    }

    m_textureAtlas->use();
    m_chunkShader->use();
    m_chunkShader->setMat4("uProjection", camera.getProjectionMatrix());
    m_chunkShader->setMat4("uView", camera.getViewMatrix());
    m_chunkShader->setMat4("uModel", glm::mat4(1.0f));
    m_chunkShader->setBool("uAOEnabled", useAO);
    m_chunkShader->setFloat("uAOIntensity", aoFactor);

    for (auto& [chunkPos, chunkMesh] : m_activeChunkMeshes)
    {
        m_chunkShader->setVec3("uChunkOffset", glm::vec3(chunkPos) * float(Chunk::CHUNK_SIZE));
        chunkMesh->draw();
    }
}

void ChunkMapRenderer::meshBuildThreadFunc(const gfx::TextureAtlas<BlockTexture>& atlas, bool useSmoothLighting) {
    while (!m_stopThread)
    {
        ChunkSnapshot snapshot;
        if (!m_chunksToBuild.popFrontNoWait(snapshot)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
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