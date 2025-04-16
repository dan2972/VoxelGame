#include "world/block_data.h"
#include <stdexcept>
#include <spdlog/spdlog.h>

std::string blockTypeToString(BlockType type)
{
    if (static_cast<int>(type) < 0 || static_cast<int>(type) >= blockTypeNames.size())
    {
        spdlog::error("BlockType out of range: {}", static_cast<int>(type));
        throw std::out_of_range("BlockType out of range");
    }
    return blockTypeNames[static_cast<int>(type)];
}

bool isTransparentBlock(BlockType type)
{
    if (type == BlockType::Air || type == BlockType::Water)
    {
        return true;
    }
    return false;
}