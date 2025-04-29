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
        node.chunkMesh->setDirty(false);

        m_chunkMeshes[node.chunkPos] = node.chunkMesh;
        m_activeChunkMeshes[node.chunkPos] = node.chunkMesh;
        m_chunksInBuildQueue.erase(node.chunkPos);
        meshSubmitCount++;
    }

}

void ChunkMapRenderer::queueFrustum(const Frustum& frustum, const glm::ivec3& chunkPos, int radius) 
{
    std::queue<glm::ivec3> nodes;
    std::unordered_set<glm::ivec3, glm_ivec3_hash, glm_ivec3_equal> visited;
    nodes.push(chunkPos);
    visited.insert(chunkPos);
    while(!nodes.empty())
    {
        glm::ivec3 node = nodes.front();
        nodes.pop();

        if (m_chunkMeshes.contains(node)) {
            m_activeChunkMeshes[node] = m_chunkMeshes[node];
        } else if (!m_chunksInBuildQueue.contains(node)) {
            std::vector<glm::ivec3> failedChunks;
            auto snapshot = ChunkSnapshot::CreateSnapshot(*m_chunkMap, node, &failedChunks);
            if (snapshot) {
                glm::vec3 chunkMin = glm::vec3(node) * float(Chunk::CHUNK_SIZE);
                glm::vec3 chunkMax = chunkMin + glm::vec3(Chunk::CHUNK_SIZE);
                if (frustum.intersectsAABB(chunkMin, chunkMax) && !snapshot->center()->isAllAir() && m_chunksToBuild.size() < MAX_BUILD_QUEUE_SIZE) {
                    m_chunksInBuildQueue.insert(node);
                    m_chunksToBuild.pushBack(snapshot.value());
                }
            } else {
                for (const auto& failedChunk : failedChunks) {
                    m_chunkMap->queueChunk(failedChunk);
                }
                continue;
            }
        }
        
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dz = -1; dz <= 1; ++dz) {
                    if (dx == 0 && dy == 0 && dz == 0) continue;
                    glm::ivec3 neighborPos = node + glm::ivec3(dx, dy, dz);
                    glm::vec3 dOrigin = neighborPos - chunkPos;
                    float distance2 = dOrigin.x * dOrigin.x + dOrigin.y * dOrigin.y + dOrigin.z * dOrigin.z;
                    
                    if (distance2 > radius * radius) continue;
                    if (visited.contains(neighborPos)) continue;
                    
                    glm::vec3 chunkMin = glm::vec3(neighborPos) * float(Chunk::CHUNK_SIZE);
                    glm::vec3 chunkMax = chunkMin + glm::vec3(Chunk::CHUNK_SIZE);
                    if (!frustum.intersectsAABB(chunkMin, chunkMax)) continue;
                    nodes.push(neighborPos);
                    visited.insert(neighborPos);
                }
            }
        }
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
        
        auto snapshot = ChunkSnapshot::CreateSnapshot(*m_chunkMap, pos);
        if (!snapshot)
            continue;
        m_chunksInBuildQueue.insert(pos);
        m_chunksToBuild.pushBack(snapshot.value());
    }
}

void ChunkMapRenderer::queueBlockUpdate(const glm::ivec3& blockPos, BlockType blockType)
{
    glm::ivec3 chunkPos = Chunk::globalToChunkPos(blockPos);

    static std::vector<glm::ivec3> poses = {
        {-1,0,0}, {1,0,0},
        {0,-1,0}, {0,1,0},
        {0,0,-1}, {0,0,1},
        {-1,1,0}, {1,1,0}, {1,-1,0}, {-1,-1,0},
        {-1,0,1}, {1,0,1}, {1,0,-1}, {-1,0,-1},
        {0,-1,1}, {0,1,1}, {0,1,-1}, {0,-1,-1},
        {-1,-1,-1}, {-1,-1,1}, {-1,1,-1}, {1,-1,-1},
        {1,-1,1}, {1,1,-1}, {-1,1,1}, {1,1,1}
    };

    for (int i = poses.size()-1; i >= 0; i--) {
        glm::ivec3 neighborChunkPos = chunkPos + poses[i];
        setDirty(neighborChunkPos);
    }

    // block light calculation
    uint8_t curLight = m_chunkMap->getBlockLight(blockPos);
    uint8_t newLight = BlockData::getLuminosity(blockType);
    if (newLight > curLight) {
        m_chunkMap->addLights(chunkPos, {{blockPos, newLight}}, true);
    } else if (newLight < curLight) {
        m_chunkMap->removeLights(chunkPos, {{blockPos, newLight}}, true);

    } else if (blockType == BlockType::Air) {
        if (m_chunkMap->getNearbyBlockLight(blockPos) > 1) {
            m_chunkMap->addLights(chunkPos, {{blockPos, static_cast<uint16_t>(m_chunkMap->getNearbyBlockLight(blockPos) - 1)}}, true);
        }
    }

    // sunlight calculation
    if (blockType == BlockType::Air) {
        if (m_chunkMap->getSunLight({blockPos.x, blockPos.y + 1, blockPos.z}) == 15) {
            m_chunkMap->addLights(chunkPos, {{blockPos, 15}}, false);
        } else if (m_chunkMap->getNearbySkyLight(blockPos) > 1) {
            m_chunkMap->addLights(chunkPos, {{blockPos, static_cast<uint16_t>(m_chunkMap->getNearbySkyLight(blockPos) - 1)}}, false);
        }
    } else if (BlockData::isOpaqueBlock(blockType)) {
        if (m_chunkMap->getNearbySkyLight(blockPos) > 1) {
            m_chunkMap->removeLights(chunkPos, {{blockPos, 0}}, false);
        }
    }
    
    setDirty(chunkPos);
}

void ChunkMapRenderer::draw(const Camera& camera, int viewDistance, bool useAO, float aoFactor, float dayNightFrac)
{
    checkPointers();

    glm::ivec3 cameraChunkPos = Chunk::globalToChunkPos(camera.position);
    Frustum frustum = camera.getFrustum();
    std::queue<glm::ivec3> chunksToUnLoad;
    for (auto& [chunkPos, chunkMesh] : m_activeChunkMeshes)
    {
        if (chunkMesh->isDirty() && !m_chunksInBuildQueue.contains(chunkPos)) {
            auto snapshot = ChunkSnapshot::CreateSnapshot(*m_chunkMap, chunkPos);
            if (snapshot) {
                m_chunksToBuild.pushFront(snapshot.value());
                m_chunksInBuildQueue.insert(chunkPos);
            }
        }

        glm::ivec3 delta = chunkPos - cameraChunkPos;
        float distance2 = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        if (distance2 > viewDistance * viewDistance) {
            chunksToUnLoad.push(chunkPos);
            continue;
        }
        glm::vec3 chunkMin = glm::vec3(chunkPos) * float(Chunk::CHUNK_SIZE);
        glm::vec3 chunkMax = chunkMin + glm::vec3(Chunk::CHUNK_SIZE);
        if (!frustum.intersectsAABB(chunkMin, chunkMax)) {
            chunksToUnLoad.push(chunkPos);
            continue;
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
    m_chunkShader->setFloat("uDayNightFrac", dayNightFrac);

    for (auto& [chunkPos, chunkMesh] : m_activeChunkMeshes)
    {
        m_chunkShader->setVec3("uChunkOffset", glm::vec3(chunkPos) * float(Chunk::CHUNK_SIZE));
        chunkMesh->draw(RenderLayer::Opaque);
    }

    for (auto& [chunkPos, chunkMesh] : m_activeChunkMeshes)
    {
        m_chunkShader->setVec3("uChunkOffset", glm::vec3(chunkPos) * float(Chunk::CHUNK_SIZE));
        chunkMesh->draw(RenderLayer::Translucent);
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

inline void ChunkMapRenderer::setDirty(const glm::ivec3& chunkPos)
{
    auto it = m_chunkMeshes.find(chunkPos);
    if (it != m_chunkMeshes.end()) {
        it->second->setDirty(true);
    }
}