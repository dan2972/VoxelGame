#pragma once

#include <glm/glm.hpp>
#include "utils/geometry.h"

enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera
{
public:
    static const float DEFAULT_YAW;
    static const float DEFAULT_PITCH;
    static const float DEFAULT_SPEED;
    static const float DEFAULT_SENSITIVITY;
    static const float DEFAULT_ZOOM;

    float movementSpeed;
    float mouseSensitivity;
    float zoom;

    glm::ivec2 resolution{800, 600};
    glm::ivec2 framebufferSize{800, 600};

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw, pitch;

    bool freezeFrustum = false;

    Camera(
        const glm::vec3& position=glm::vec3(0.0f, 0.0f, 0.0f), 
        const glm::vec3& up=glm::vec3(0.0f, 1.0f, 0.0f), 
        float yaw=Camera::DEFAULT_YAW, 
        float pitch=Camera::DEFAULT_PITCH
    );

    void move(CameraMovement direction, float deltaTime);
    void rotate(float xoffset, float yoffset, bool constrainPitch=true);
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getViewMatrix() const;
    void updateResolution(float width, float height);
    void updateFramebufferSize(float width, float height);
    glm::vec3 rayDirFromNDC(float x, float y);
    glm::vec3 rayDirFromMouse(float mouseX, float mouseY);
    Frustum getFrustum() const;
private:
    Frustum m_frustum;
    void updateCameraVectors();
    glm::vec2 getNormalizedDeviceCoords(float mouseX, float mouseY);
    glm::vec4 toEyeCoords(const glm::vec4& clipCoords);
    glm::vec3 toWorldCoords(const glm::vec4& eyeCoords);
};