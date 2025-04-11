#pragma once

#include <glm/glm.hpp>

enum class BlockFace
{
    FRONT = 0,
    BACK = 1,
    LEFT = 2,
    RIGHT = 3,
    TOP = 4,
    BOTTOM = 5
};

class DirectionUtils
{
public:
    static const glm::vec3 CARDINAL_DIRECTIONS[];
    static glm::vec3 blockfaceDirection(BlockFace face);
};