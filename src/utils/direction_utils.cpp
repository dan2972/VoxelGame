#include "utils/direction_utils.h"

const glm::vec3 DirectionUtils::CARDINAL_DIRECTIONS[] = 
{
    glm::vec3(0.0f, 1.0f, 0.0f), // TOP
    glm::vec3(0.0f, -1.0f, 0.0f), // BOTTOM
    glm::vec3(0.0f, 0.0f, 1.0f), // FRONT
    glm::vec3(0.0f, 0.0f, -1.0f), // BACK
    glm::vec3(-1.0f, 0.0f, 0.0f), // LEFT
    glm::vec3(1.0f, 0.0f, 0.0f) // RIGHT
};

glm::vec3 DirectionUtils::blockfaceDirection(BlockFace face)
{
    return CARDINAL_DIRECTIONS[static_cast<int>(face)];
}