#include "graphics/chunk_mesh.h"
#include "game_application.h"

void ChunkMesh::setup()
{
    if (m_indexCounter != 0)
        m_mesh.populate(m_vertices, m_indices, {1, 2});
    if (m_indexCounterTranslucent != 0)
        m_meshTranslucent.populate(m_verticesTranslucent, m_indicesTranslucent, {1, 2});
    if (m_indexCounterTransparent != 0)
        m_meshTransparent.populate(m_verticesTransparent, m_indicesTransparent, {1, 2});
}

void ChunkMesh::draw(RenderLayer layer)
{
    switch (layer)
    {
        case RenderLayer::Opaque:
            if (m_indexCounter != 0)
                m_mesh.draw();
            break;
        case RenderLayer::Translucent:
            if (m_indexCounterTranslucent != 0)
                m_meshTranslucent.draw();
            break;
        case RenderLayer::Transparent:
            if (m_indexCounterTransparent != 0)
                m_meshTransparent.draw();
            break;
        default:
            return; // Invalid layer
    }
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

    m_verticesTranslucent.clear();
    m_indicesTranslucent.clear();
    m_indexCounterTranslucent = 0;

    m_verticesTransparent.clear();
    m_indicesTransparent.clear();
    m_indexCounterTransparent = 0;
}

void ChunkMesh::buildMesh(const ChunkSnapshot& snapshot, const gfx::TextureAtlas<BlockTexture>& atlas, bool smoothLighting)
{
    if (!snapshot.isValid())
        return;
    if (snapshot.center()->isAllAir())
        return;
    clearMesh();
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
                BlockTextureData blockTextureData = BlockData::getBlockTextureData(blockType);
                std::array<float, 8> textureCoords;
                bool texureFaceAllSame = blockTextureData.allSame();
                if (texureFaceAllSame) {
                    blockTexture = blockTextureData.getTexture(BlockFace::Top);
                    auto [uvMin, uvMax] = atlas.get(blockTexture);

                    textureCoords = 
                    {
                        uvMin.x, uvMax.y,
                        uvMax.x, uvMax.y,
                        uvMax.x, uvMin.y,
                        uvMin.x, uvMin.y
                    };
                }

                RenderLayer layer = getRenderLayer(blockType);

                for (int i = 0; i < 6; ++i)
                {
                    if (!texureFaceAllSame) {
                        blockTexture = blockTextureData.getTexture(static_cast<BlockFace>(i));
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
                        addFace(pos, static_cast<BlockFace>(i), textureCoords, aoValues, lightValues, layer, flipQuad);
                    }
                }
            }
        }
    }
}

void ChunkMesh::submitBuffers()
{
    if (m_indexCounter != 0)
        m_mesh.updateBuffers(m_vertices, m_indices);
    if (m_indexCounterTranslucent != 0)
        m_meshTranslucent.updateBuffers(m_verticesTranslucent, m_indicesTranslucent);
    if (m_indexCounterTransparent != 0)
        m_meshTransparent.updateBuffers(m_verticesTransparent, m_indicesTransparent);
}

void ChunkMesh::addFace
(
    const glm::ivec3 &pos, 
    BlockFace face, 
    std::array<float, 8> texCoords, 
    std::array<int, 4> aoValues, 
    std::array<glm::vec4, 4> lightLevels, 
    RenderLayer layer,
    bool flipQuad
)
{
    std::vector<float>* vertices;
    std::vector<unsigned int>* indices;
    unsigned int* indexCounter;
    switch (layer)
    {
        case RenderLayer::Opaque:
            vertices = &m_vertices;
            indices = &m_indices;
            indexCounter = &m_indexCounter;
            break;
        case RenderLayer::Translucent:
            vertices = &m_verticesTranslucent;
            indices = &m_indicesTranslucent;
            indexCounter = &m_indexCounterTranslucent;
            break;
        case RenderLayer::Transparent:
            vertices = &m_verticesTransparent;
            indices = &m_indicesTransparent;
            indexCounter = &m_indexCounterTransparent;
            break;
        default:
            return; // Invalid layer
    }

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
        // 4 bits for the light levels (0-15)
        vPacked = (vPacked << 4) + static_cast<uint32_t>(lightLevels[i].a); // sun light
        vPacked = (vPacked << 4) + static_cast<uint32_t>(lightLevels[i].b); // block light
        vertices->push_back(std::bit_cast<float>(vPacked));

        vertices->push_back(texCoords[texIndex++]);
        vertices->push_back(texCoords[texIndex++]);
    }

    if (flipQuad)
    {
        indices->push_back(*indexCounter + 3);
        indices->push_back(*indexCounter + 0);
        indices->push_back(*indexCounter + 1);

        indices->push_back(*indexCounter + 1);
        indices->push_back(*indexCounter + 2);
        indices->push_back(*indexCounter + 3);
    }
    else
    {
        indices->push_back(*indexCounter + 0);
        indices->push_back(*indexCounter + 1);
        indices->push_back(*indexCounter + 2);

        indices->push_back(*indexCounter + 2);
        indices->push_back(*indexCounter + 3);
        indices->push_back(*indexCounter + 0);
    }

    (*indexCounter) += 4;
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
        bool side1 = !(BlockData::isTransparentBlock(s1B) || BlockData::isTranslucentBlock(s1B));
        bool side2 = !(BlockData::isTransparentBlock(s2B) || BlockData::isTranslucentBlock(s2B));
        bool cornerBlock = !(BlockData::isTransparentBlock(cB) || BlockData::isTranslucentBlock(cB));
        aoValues[i] = vertexAO(side1, side2, cornerBlock);
    }
    return aoValues;
}



std::array<glm::vec4, 4> ChunkMesh::getLightValues(const glm::ivec3 &blockPos, BlockFace face, const ChunkSnapshot& snapshot, bool smoothLighting)
{
    std::array<glm::vec4, 4> lightValues;
    auto faceCoords = getFaceCoords(face);

    glm::ivec3 curLightPos = blockPos + static_cast<glm::ivec3>(DirectionUtils::blockfaceDirection(face));

    for (int i = 0; i < 4; ++i)
    {
        glm::vec4 currentLight = glm::vec4(0,0,snapshot.getBlockLightFromLocalPos(curLightPos), snapshot.getSunLightFromLocalPos(curLightPos));
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

        glm::vec4 side1 = glm::vec4(0,0,snapshot.getBlockLightFromLocalPos(blockPos + s1),snapshot.getSunLightFromLocalPos(blockPos + s1));
        glm::vec4 side2 = glm::vec4(0,0,snapshot.getBlockLightFromLocalPos(blockPos + s2),snapshot.getSunLightFromLocalPos(blockPos + s2));
        glm::vec4 cornerBlock = glm::vec4(0,0,snapshot.getBlockLightFromLocalPos(blockPos + c),snapshot.getSunLightFromLocalPos(blockPos + c));

        bool bs1 = snapshot.getBlockFromLocalPos(blockPos + s1) == BlockType::Air;
        bool bs2 = snapshot.getBlockFromLocalPos(blockPos + s2) == BlockType::Air;
        bool bc = snapshot.getBlockFromLocalPos(blockPos + c) == BlockType::Air;

        for (int j = 0; j < 4; ++j)
        {
            float divisor = 1 + (bs1 ? 1 : 0) + (bs2 ? 1 : 0);
            float dividend = currentLight[j] + (bs1 ? side1[j] : 0) + (bs2 ? side2[j] : 0);
            if (bs1 || bs2)
            {
                divisor += bc ? 1 : 0;
                dividend += bc ? cornerBlock[j] : 0;
            }
            lightValues[i][j] = dividend / divisor;
        }
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
    return curBlock != neighbor && (BlockData::isTransparentBlock(neighbor) || BlockData::isTranslucentBlock(neighbor));
}

RenderLayer ChunkMesh::getRenderLayer(BlockType blockType)
{
    auto blockData = BlockData::getBlockData(blockType);
    if (blockData.isTranslucent)
        return RenderLayer::Translucent;
    if (blockData.isTransparent)
        return RenderLayer::Transparent;
    return RenderLayer::Opaque;
}