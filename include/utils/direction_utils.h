#pragma once

#include <glm/glm.hpp>
#include "world/block_data.h"

class DirectionUtils
{
public:
    static const glm::vec3 CARDINAL_DIRECTIONS[];
    static glm::vec3 blockfaceDirection(BlockFace face);
};