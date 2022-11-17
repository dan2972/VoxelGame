#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float DEFAULT_YAW = -90.0f;
const float DEFAULT_PITCH = 0.0f;
const float DEFAULT_SPEED = 5.50f;
const float DEFAULT_SENSITIVITY = 0.1f;
const float DEFAULT_ZOOM = 45.0f;

class Camera
{
public:
	static enum CameraMovement {
		Forward,
		Backward,
		Left,
		Right,
        Up,
        Down
	};

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = DEFAULT_YAW, float pitch = DEFAULT_PITCH) 
        : m_front{ glm::vec3(0.0f, 0.0f, -1.0f) }, m_movementSpeed{ DEFAULT_SPEED }, m_mouseSensitivity{ DEFAULT_SENSITIVITY }, m_zoom{ DEFAULT_ZOOM },
        m_position{ position }, m_worldUp{ glm::normalize(up) }, m_yaw{ yaw }, m_pitch{ pitch }
    {
        updateCameraVectors();
	}

    glm::mat4 getViewMatrix();

    float getZoom();

    void ProcessKeyboard(CameraMovement direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
private:
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    float m_yaw;
    float m_pitch;

    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_zoom;

    void updateCameraVectors();
};

