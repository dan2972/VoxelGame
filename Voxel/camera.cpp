#include "camera.h"

glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(m_position, m_position + m_front, m_up);
}

float Camera::getZoom() {
    return m_zoom;
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime) {
    float velocity = m_movementSpeed * deltaTime;
    glm::vec3 forward = m_front * velocity;
    if (direction == Forward) {
        m_position.x += forward.x;
        m_position.z += forward.z;
    } if (direction == Backward) {
        m_position.x -= forward.x;
        m_position.z -= forward.z;
    } if (direction == Left) {
        m_position -= m_right * velocity;
    } if (direction == Right) {
        m_position += m_right * velocity;
    } if (direction == Up) {
        m_position += m_worldUp * velocity;
    } if (direction == Down) {
        m_position -= m_worldUp * velocity;
    }
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yaw += xoffset;
    m_pitch += yoffset;

    if (constrainPitch)
    {
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 front{};
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}