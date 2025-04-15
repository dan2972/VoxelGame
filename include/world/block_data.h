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
};

static const std::array<std::string, 5> blockTypeNames = {
    "air",
    "grass",
    "dirt",
    "stone",
    "wood_planks"
};

std::string blockTypeToString(BlockType type);