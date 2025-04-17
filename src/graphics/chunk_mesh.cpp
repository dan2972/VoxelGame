#include "graphics/chunk_mesh.h"
#include "game_application.h"

void ChunkMesh::setup()
{
    m_mesh.populate(m_vertices, m_indices, {1, 2});
}

void ChunkMesh::draw()
{
    if (m_indexCounter == 0)
        return;
    m_mesh.draw();
}

bool inBounds(const glm::ivec3& pos) {
    if (pos.x < 0 || pos.x >= Chunk::CHUNK_SIZE || pos.y < 0 || pos.y >= Chunk::CHUNK_SIZE || pos.z < 0 || pos.z >= Chunk::CHUNK_SIZE)
        return false;
    return true;
}

void ChunkMesh::clearMesh()
{
    m_vertices.clear();
    m_indices.clear();
    m_indexCounter = 0;
}

void ChunkMesh::buildMesh(const ChunkSnapshot& snapshot, const gfx::TextureAtlas<BlockTexture>& atlas, bool smoothLighting)
{
    if (!snapshot.isValid())
        return;
    if (snapshot.center()->isAllAir())
        return;
    m_vertices.clear();
    m_indices.clear();
    m_indexCounter = 0;
    for (int x = 0; x < Chunk::CHUNK_SIZE; ++x)
    {
        for (int z = 0; z < Chunk::CHUNK_SIZE; ++z)
        {
            for (int y = 0; y < Chunk::CHUNK_SIZE; ++y)
            {
                glm::ivec3 pos{x, y, z};
                BlockType blockType = snapshot.center()->getBlock(pos);
                if (blockType == BlockType::Air)
                    continue;
                
                BlockTexture blockTexture;
                std::array<float, 8> textureCoords;
                bool texureFaceAllSame = BlockData::getBlockTextureData(blockType).allSame();
                if (texureFaceAllSame) {
                    blockTexture = BlockData::getBlockTexture(blockType, BlockFace::Top);
                    auto [uvMin, uvMax] = atlas.get(blockTexture);

                    textureCoords = 
                    {
                        uvMin.x, uvMax.y,
                        uvMax.x, uvMax.y,
                        uvMax.x, uvMin.y,
                        uvMin.x, uvMin.y
                    };
                }

                for (int i = 0; i < 6; ++i)
                {
                    if (!texureFaceAllSame) {
                        blockTexture = BlockData::getBlockTexture(blockType, static_cast<BlockFace>(i));
                        auto [uvMin, uvMax] = atlas.get(blockTexture);

                        textureCoords = 
                        {
                            uvMin.x, uvMax.y,
                            uvMax.x, uvMax.y,
                            uvMax.x, uvMin.y,
                            uvMin.x, uvMin.y
                        };
                    }

                    glm::ivec3 dir = static_cast<glm::ivec3>(DirectionUtils::blockfaceDirection(static_cast<BlockFace>(i)));
                    BlockType bType = snapshot.getBlockFromLocalPos(pos + dir);
                    if (shouldRenderFace(blockType, bType))
                    {
                        auto aoValues = getAOValues(pos, static_cast<BlockFace>(i), snapshot);
                        auto lightValues = getLightValues(pos, static_cast<BlockFace>(i), snapshot, smoothLighting);
                        bool flipQuad = shouldFlipQuad(aoValues);
                        addFace(pos, static_cast<BlockFace>(i), textureCoords, aoValues, lightValues, flipQuad);
                    }
                }
            }
        }
    }
}

void ChunkMesh::submitBuffers()
{
    m_mesh.updateBuffers(m_vertices, m_indices);
}

void ChunkMesh::addFace
(
    const glm::ivec3 &pos, 
    BlockFace face, 
    std::array<float, 8> texCoords, 
    std::array<int, 4> aoValues, 
    std::array<float, 4> lightLevels, 
    bool flipQuad
)
{
    auto faceCoords = getFaceCoords(face);
    for (int i = 0, vertIndex = 0, texIndex = 0; i < 4; ++i)
    {
        // each local position dimension can be packed into 6 bits (0-63)
        uint32_t vPacked = pos.x + faceCoords[vertIndex++];
        vPacked = (vPacked << 6) + pos.y + faceCoords[vertIndex++];
        vPacked = (vPacked << 6) + pos.z + faceCoords[vertIndex++];
        // 3 bits for the normal index (0-7)
        vPacked = (vPacked << 3) + static_cast<uint32_t>(face);
        // 2 bits for the AO value (0-3)
        vPacked = (vPacked << 2) + static_cast<uint32_t>(aoValues[i]);
        // 4 bits for the light level (0-15)
        vPacked = (vPacked << 4) + static_cast<uint32_t>(lightLevels[i]);
        m_vertices.push_back(std::bit_cast<float>(vPacked));

        m_vertices.push_back(texCoords[texIndex++]);
        m_vertices.push_back(texCoords[texIndex++]);
    }

    if (flipQuad)
    {
        m_indices.push_back(m_indexCounter + 3);
        m_indices.push_back(m_indexCounter + 0);
        m_indices.push_back(m_indexCounter + 1);

        m_indices.push_back(m_indexCounter + 1);
        m_indices.push_back(m_indexCounter + 2);
        m_indices.push_back(m_indexCounter + 3);
    }
    else
    {
        m_indices.push_back(m_indexCounter + 0);
        m_indices.push_back(m_indexCounter + 1);
        m_indices.push_back(m_indexCounter + 2);

        m_indices.push_back(m_indexCounter + 2);
        m_indices.push_back(m_indexCounter + 3);
        m_indices.push_back(m_indexCounter + 0);
    }

    m_indexCounter += 4;
}

std::array<int, 12> ChunkMesh::getFaceCoords(BlockFace face)
{
    switch(face)
    {
        case BlockFace::Front:
            return {0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1};
        case BlockFace::Back:
            return {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0};
        case BlockFace::Left:
            return {0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0};
        case BlockFace::Right:
            return {1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1};
        case BlockFace::Top:
            return {0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0};
        case BlockFace::Bottom:
            return {0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1};
        default:
            return {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    }
}

std::array<int, 4> ChunkMesh::getAOValues(const glm::ivec3 &blockPos, BlockFace face, const ChunkSnapshot& snapshot)
{
    std::array<int, 4> aoValues;
    auto faceCoords = getFaceCoords(face);
    for (int i = 0; i < 4; ++i)
    {
        glm::ivec3 corner = glm::ivec3
        {
            faceCoords[i * 3],
            faceCoords[i * 3 + 1],
            faceCoords[i * 3 + 2]
        };
        glm::ivec3 s1, s2, c;
        getAOBlockPos(corner, face, &s1, &s2, &c);
        BlockType s1B, s2B, cB;
        s1B = snapshot.getBlockFromLocalPos(blockPos + s1);
        s2B = snapshot.getBlockFromLocalPos(blockPos + s2);
        cB = snapshot.getBlockFromLocalPos(blockPos + c);
        bool side1 = s1B != BlockType::Air;
        bool side2 = s2B != BlockType::Air;
        bool cornerBlock = cB != BlockType::Air;
        aoValues[i] = vertexAO(side1, side2, cornerBlock);
    }
    return aoValues;
}



std::array<float, 4> ChunkMesh::getLightValues(const glm::ivec3 &blockPos, BlockFace face, const ChunkSnapshot& snapshot, bool smoothLighting)
{
    std::array<float, 4> lightValues;
    auto faceCoords = getFaceCoords(face);

    glm::ivec3 curLightPos = blockPos + static_cast<glm::ivec3>(DirectionUtils::blockfaceDirection(face));
    unsigned char currentLight = snapshot.getLightLevelFromLocalPos(curLightPos);

    for (int i = 0; i < 4; ++i)
    {
        if (!smoothLighting)
        {
            lightValues[i] = currentLight;
            continue;
        }

        glm::ivec3 corner = glm::ivec3
        {
            faceCoords[i * 3],
            faceCoords[i * 3 + 1],
            faceCoords[i * 3 + 2]
        };
        glm::ivec3 s1, s2, c;
        getAOBlockPos(corner, face, &s1, &s2, &c);

        unsigned char side1 = snapshot.getLightLevelFromLocalPos(blockPos + s1);
        unsigned char side2 = snapshot.getLightLevelFromLocalPos(blockPos + s2);
        unsigned char cornerBlock = snapshot.getLightLevelFromLocalPos(blockPos + c);

        bool bs1 = snapshot.getBlockFromLocalPos(blockPos + s1) == BlockType::Air;
        bool bs2 = snapshot.getBlockFromLocalPos(blockPos + s2) == BlockType::Air;
        bool bc = snapshot.getBlockFromLocalPos(blockPos + c) == BlockType::Air;

        float divisor = 1 + (bs1 ? 1 : 0) + (bs2 ? 1 : 0);
        float dividend = currentLight + (bs1 ? side1 : 0) + (bs2 ? side2 : 0);
        if (bs1 || bs2)
        {
            divisor += bc ? 1 : 0;
            dividend += bc ? cornerBlock : 0;
        }
        lightValues[i] = dividend / divisor;
    }
    return lightValues;
}

bool ChunkMesh::shouldFlipQuad(std::array<int, 4> aoValues)
{
    return (aoValues[0] + aoValues[2]) < (aoValues[1] + aoValues[3]);
}

void ChunkMesh::getAOBlockPos(const glm::ivec3 &cornerPos, BlockFace face, glm::ivec3 *outs1, glm::ivec3 *outs2, glm::ivec3 *outc)
{
    glm::ivec3 s1, s2, c;
    if (face == BlockFace::Left || face == BlockFace::Right)
        s1.x = cornerPos.x == 0 ? -1 : 1;
    else
        s1.x = 0;
    c.x = cornerPos.x == 0 ? -1 : 1;
    s2.x = cornerPos.x == 0 ? -1 : 1;
    
    
    if (face == BlockFace::Left || face == BlockFace::Right)
        s1.y = 0;
    else
        s1.y = cornerPos.y == 0 ? -1 : 1;
    c.y = cornerPos.y == 0 ? -1 : 1;
    if (face == BlockFace::Front || face == BlockFace::Back)
        s2.y = 0;
    else
        s2.y = cornerPos.y == 0 ? -1 : 1;
    
    
    s1.z = cornerPos.z == 0 ? -1 : 1;
    c.z = cornerPos.z == 0 ? -1 : 1;
    if (face == BlockFace::Front || face == BlockFace::Back)
        s2.z = cornerPos.z == 0 ? -1 : 1;
    else
        s2.z = 0;
    
    *outs1 = s1;
    *outs2 = s2;
    *outc = c;
}

int ChunkMesh::vertexAO(bool side1, bool side2, bool corner)
{
    if (side1 && side2)
        return 0;
    return 3 - (side1 + side2 + corner);
}

bool ChunkMesh::shouldRenderFace(BlockType curBlock, BlockType neighbor)
{
    return curBlock != neighbor && BlockData::isTransparentBlock(neighbor);
}