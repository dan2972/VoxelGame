#pragma once

#include <glm/glm.hpp>
#include <array>

struct Plane {
    glm::vec3 normal;
    float d;

    float distanceToPoint(const glm::vec3& p) const {
        return glm::dot(normal, p) + d;
    }
};

class Frustum 
{
public:
    std::array<Plane, 6> planes;

    Frustum() = default;
    Frustum(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);

    bool AABBInFrustum(const glm::vec3& min, const glm::vec3& max) const;
};