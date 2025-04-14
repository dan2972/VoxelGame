#pragma once

#include <string>
#include <array>
#include <cstdint>

enum class BlockType : uint16_t
{
    Air = 0,
    Grass = 1,
    Dirt = 2,
    Stone = 3,
};

static const std::array<std::string, 4> blockTypeNames = {
    "air",
    "grass",
    "dirt",
    "stone",
};

std::string blockTypeToString(BlockType type);