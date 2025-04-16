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
    WoodPlanks = 4,
    Sand = 5,
    Water = 6,
};

static const std::array<std::string, 7> blockTypeNames = {
    "air",
    "grass",
    "dirt",
    "stone",
    "wood_planks",
    "sand",
    "water",
};

std::string blockTypeToString(BlockType type);
bool isTransparentBlock(BlockType type);