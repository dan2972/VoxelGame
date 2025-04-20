#include "utils/geometry.h"

Frustum::Frustum(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) {
    glm::mat4 clipMatrix = projectionMatrix * viewMatrix;
    planes[0].normal = glm::vec3(clipMatrix[0][3] + clipMatrix[0][0], clipMatrix[1][3] + clipMatrix[1][0], clipMatrix[2][3] + clipMatrix[2][0]);
    planes[0].d = clipMatrix[3][3] + clipMatrix[3][0];

    planes[1].normal = glm::vec3(clipMatrix[0][3] - clipMatrix[0][0], clipMatrix[1][3] - clipMatrix[1][0], clipMatrix[2][3] - clipMatrix[2][0]);
    planes[1].d = clipMatrix[3][3] - clipMatrix[3][0];

    planes[2].normal = glm::vec3(clipMatrix[0][3] + clipMatrix[0][1], clipMatrix[1][3] + clipMatrix[1][1], clipMatrix[2][3] + clipMatrix[2][1]);
    planes[2].d = clipMatrix[3][3] + clipMatrix[3][1];

    planes[3].normal = glm::vec3(clipMatrix[0][3] - clipMatrix[0][1], clipMatrix[1][3] - clipMatrix[1][1], clipMatrix[2][3] - clipMatrix[2][1]);
    planes[3].d = clipMatrix[3][3] - clipMatrix[3][1];

    planes[4].normal = glm::vec3(clipMatrix[0][3] + clipMatrix[0][2], clipMatrix[1][3] + clipMatrix[1][2], clipMatrix[2][3] + clipMatrix[2][2]);
    planes[4].d = clipMatrix[3][3] + clipMatrix[3][2];

    planes[5].normal = glm::vec3(clipMatrix[0][3] - clipMatrix[0][2], clipMatrix[1][3] - clipMatrix[1][2], clipMatrix[2][3] - clipMatrix[2][2]);
    planes[5].d = clipMatrix[3][3] - clipMatrix[3][2];

    for (auto& plane : planes) {
        float length = glm::length(plane.normal);
        if (length > 0) {
            plane.normal /= length;
            plane.d /= length;
        }
    }
}

bool Frustum::intersectsAABB(const glm::vec3& min, const glm::vec3& max) const {
    for (const auto& plane : planes) {
        glm::vec3 positiveVertex = min;
        if (plane.normal.x >= 0) positiveVertex.x = max.x;
        if (plane.normal.y >= 0) positiveVertex.y = max.y;
        if (plane.normal.z >= 0) positiveVertex.z = max.z;

        if (plane.distanceToPoint(positiveVertex) < 0) {
            return false;
        }
    }
    return true;
}