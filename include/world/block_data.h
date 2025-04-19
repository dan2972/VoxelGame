#pragma once

#include <string>
#include <array>
#include <cstdint>
#include <unordered_map>
#include <stdexcept>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

enum class BlockFace
{
    Front = 0,
    Back = 1,
    Left = 2,
    Right = 3,
    Top = 4,
    Bottom = 5
};

enum class BlockType : uint16_t
{
    Air = 0,
    Grass = 1,
    Dirt = 2,
    Stone = 3,
    WoodPlanks = 4,
    Sand = 5,
    Water = 6,
};

enum class BlockTexture : uint16_t
{
    GrassTop = 0,
    GrassSide = 1,
    Dirt = 2,
    Stone = 3,
    WoodPlanks = 4,
    Sand = 5,
    Water = 6,
};

struct BlockTextureData
{
    BlockTexture top;
    BlockTexture bottom;
    BlockTexture front;
    BlockTexture back;
    BlockTexture left;
    BlockTexture right;

    bool allSame () const { return top == bottom && top == front && top == back && top == left && top == right; }
    BlockTexture& getTexture(BlockFace face)
    {
        switch (face)
        {
            case BlockFace::Front: return front;
            case BlockFace::Back: return back;
            case BlockFace::Left: return left;
            case BlockFace::Right: return right;
            case BlockFace::Top: return top;
            case BlockFace::Bottom: return bottom;
            default:
                spdlog::error("Invalid block face: {}", static_cast<int>(face));
                throw std::invalid_argument("Invalid block face.");
        }
    }

    BlockTextureData()
        : top(BlockTexture::GrassTop), bottom(BlockTexture::GrassTop),
          front(BlockTexture::GrassTop), back(BlockTexture::GrassTop),
          left(BlockTexture::GrassTop), right(BlockTexture::GrassTop) {}

    BlockTextureData(BlockTexture top, BlockTexture bottom, BlockTexture front, BlockTexture back, BlockTexture left, BlockTexture right)
        : top(top), bottom(bottom), front(front), back(back), left(left), right(right) {}
    
    BlockTextureData(BlockTexture texture)
        : top(texture), bottom(texture), front(texture), back(texture), left(texture), right(texture) {}
};

struct BlockProperties
{
    bool isTransparent = false;
    bool isLiquid = false;
    bool isCube = true;
};

class BlockData
{
public:
    static std::unordered_map<std::string, BlockTexture> stringToBlockTexture;

    static void submitBlockData(BlockType type)
    {
        s_blockDataMap[static_cast<int>(type)] = BlockProperties{};
    }

    static void submitBlockData(BlockType type, const BlockProperties& blockData)
    {
        s_blockDataMap[static_cast<int>(type)] = blockData;
    }

    static void submitBlockTextureData(BlockType type, const BlockTextureData& blockTextureData)
    {
        s_blockTextureMap[static_cast<int>(type)] = blockTextureData;
    }

    static const BlockTexture& getBlockTexture(BlockType type, BlockFace face)
    {
        auto it = s_blockTextureMap.find(static_cast<int>(type));
        if (it != s_blockTextureMap.end())
        {
            return it->second.getTexture(face);
        }
        spdlog::error("Block type not found in texture map: {}", static_cast<int>(type));
        throw std::runtime_error("Block type not found in texture map.");
    }

    static const BlockTextureData& getBlockTextureData(BlockType type)
    {
        auto it = s_blockTextureMap.find(static_cast<int>(type));
        if (it != s_blockTextureMap.end())
        {
            return it->second;
        }
        spdlog::error("Block type not found in texture map: {}", static_cast<int>(type));
        throw std::runtime_error("Block type not found in texture map.");
    }

    static const BlockProperties& getBlockData(BlockType type)
    {
        auto it = s_blockDataMap.find(static_cast<int>(type));
        if (it != s_blockDataMap.end())
        {
            return it->second;
        }
        spdlog::error("Block type not found in data map: {}", static_cast<int>(type));
        throw std::runtime_error("Block type not found in data map.");
    }

    static const bool isTransparentBlock(BlockType type)
    {
        auto it = s_blockDataMap.find(static_cast<int>(type));
        if (it != s_blockDataMap.end())
        {
            return it->second.isTransparent;
        }
        spdlog::error("Block type not found in data map: {}", static_cast<int>(type));
        throw std::runtime_error("Block type not found in data map.");
    }
private:
    static std::unordered_map<int, BlockTextureData> s_blockTextureMap;
    static std::unordered_map<int, BlockProperties> s_blockDataMap;
};