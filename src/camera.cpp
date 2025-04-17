#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

const float Camera::DEFAULT_YAW = -90.0f;
const float Camera::DEFAULT_PITCH = 0.0f;
const float Camera::DEFAULT_SPEED = 5.0f;
const float Camera::DEFAULT_SENSITIVITY = 0.05f;
const float Camera::DEFAULT_ZOOM = 70.0f;

Camera::Camera(const glm::vec3 &position, const glm::vec3 &up, float yaw, float pitch)
    : front(glm::vec3(0.0f, 0.0f, -1.0f)),
        movementSpeed(DEFAULT_SPEED),
        mouseSensitivity(DEFAULT_SENSITIVITY),
        zoom(DEFAULT_ZOOM),
        resolution(glm::ivec2(800, 600)),
        yaw(yaw),
        pitch(pitch),
        worldUp(up),
        position(position),
        up(up),
        right(glm::normalize(glm::cross(front, up)))
{
    updateCameraVectors();
}

void Camera::move(CameraMovement direction, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;
    if (direction == CameraMovement::FORWARD)
        position += glm::normalize(front * glm::vec3(1.0f, 0.0f, 1.0f)) * velocity;
    if (direction == CameraMovement::BACKWARD)
        position -= glm::normalize(front * glm::vec3(1.0f, 0.0f, 1.0f)) * velocity;
    if (direction == CameraMovement::LEFT)
        position -= right * velocity;
    if (direction == CameraMovement::RIGHT)
        position += right * velocity;
    if (direction == CameraMovement::UP)
        position += worldUp * velocity;
    if (direction == CameraMovement::DOWN)
        position -= worldUp * velocity;
}

void Camera::rotate(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (constrainPitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    updateCameraVectors();
}

glm::mat4 Camera::getProjectionMatrix() const
{
    return glm::perspective(glm::radians(zoom), (float)resolution.x / resolution.y, 0.1f, 5000.0f);
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position, position + front, up);
}

void Camera::updateResolution(float width, float height)
{
    resolution = glm::ivec2(width, height);
}

void Camera::updateFramebufferSize(float width, float height)
{
    framebufferSize = glm::ivec2(width, height);
}

glm::vec3 Camera::rayDirFromNDC(float x, float y)
{
    glm::vec4 clipCoords{x, y, -1.0f, 1.0f};
    glm::vec4 eyeCoords = toEyeCoords(clipCoords);
    glm::vec3 worldRay = toWorldCoords(eyeCoords);
    return worldRay;
}

glm::vec3 Camera::rayDirFromMouse(float mouseX, float mouseY)
{
    glm::vec2 normalizedCoords = getNormalizedDeviceCoords(mouseX, mouseY);
    glm::vec4 clipCoords{normalizedCoords.x, normalizedCoords.y, -1.0f, 1.0f};
    glm::vec4 eyeCoords = toEyeCoords(clipCoords);
    glm::vec3 worldRay = toWorldCoords(eyeCoords);
    return worldRay;
}

void Camera::updateCameraVectors()
{
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

glm::vec2 Camera::getNormalizedDeviceCoords(float mouseX, float mouseY)
{
    float x = (2.0f * mouseX) / resolution.x - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / resolution.y;
    return glm::vec2(x, y);
}

glm::vec4 Camera::toEyeCoords(const glm::vec4 &clipCoords)
{
    glm::mat4 invProj = glm::inverse(getProjectionMatrix());
    glm::vec4 eyeCoords = invProj * clipCoords;
    return glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
}

glm::vec3 Camera::toWorldCoords(const glm::vec4 &eyeCoords)
{
    glm::mat4 invView = glm::inverse(getViewMatrix());
    glm::vec4 rayWorld = invView * eyeCoords;
    glm::vec3 mouseRay = glm::normalize(glm::vec3(rayWorld));
    return mouseRay;
}