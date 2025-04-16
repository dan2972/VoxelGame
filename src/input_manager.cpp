#include "input_manager.h"

GameWindow* InputManager::m_window = nullptr;
glm::vec2 InputManager::m_mousePosition = { 0.0f, 0.0f };
glm::vec2 InputManager::m_lastMousePosition = { 0.0f, 0.0f };
glm::vec2 InputManager::m_mouseDelta = { 0.0f, 0.0f };
bool InputManager::m_firstUpdate = true;
std::unordered_map<Key, bool> InputManager::m_lastKeyStates;
std::unordered_map<MouseButton, bool> InputManager::m_lastMouseButtonStates;

void InputManager::setup(GameWindow* window) {
    m_window = window;
}

void InputManager::update() {
    glfwPollEvents();
    double mouseX, mouseY;
    glfwGetCursorPos(m_window->getWindow(), &mouseX, &mouseY);
    m_mousePosition = { static_cast<float>(mouseX), static_cast<float>(mouseY) };
    if (m_firstUpdate) {
        m_lastMousePosition = m_mousePosition;
        m_firstUpdate = false;
    }
    m_mouseDelta = m_mousePosition - m_lastMousePosition;
    m_lastMousePosition = m_mousePosition;
}

bool InputManager::isKeyPressed(Key key) {
    bool b = glfwGetKey(m_window->getWindow(), static_cast<int>(key)) == GLFW_PRESS;
    m_lastKeyStates[key] = b;
    return b;
}

bool InputManager::isKeyReleased(Key key) {
    bool b = glfwGetKey(m_window->getWindow(), static_cast<int>(key)) == GLFW_RELEASE;
    m_lastKeyStates[key] = b;
    return b;
}

bool InputManager::isKeyJustPressed(Key key) {
    bool b = glfwGetKey(m_window->getWindow(), static_cast<int>(key)) == GLFW_PRESS;
    bool b2;
    auto it = m_lastKeyStates.find(key);
    if (it == m_lastKeyStates.end())
        b2 = false;
    else
        b2 = it->second;
    m_lastKeyStates[key] = b;
    return b && !b2;
}

bool InputManager::isKeyJustReleased(Key key) {
    bool b = glfwGetKey(m_window->getWindow(), static_cast<int>(key)) == GLFW_RELEASE;
    bool b2;
    auto it = m_lastKeyStates.find(key);
    if (it == m_lastKeyStates.end())
        b2 = false;
    else
        b2 = it->second;
    m_lastKeyStates[key] = b;
    return b && !b2;
}

bool InputManager::isMouseButtonPressed(MouseButton button) {
    bool b = glfwGetMouseButton(m_window->getWindow(), static_cast<int>(button)) == GLFW_PRESS;
    m_lastMouseButtonStates[button] = b;
    return b;
}

bool InputManager::isMouseButtonReleased(MouseButton button) {
    bool b = glfwGetMouseButton(m_window->getWindow(), static_cast<int>(button)) == GLFW_RELEASE;
    m_lastMouseButtonStates[button] = b;
    return b;
}

bool InputManager::isMouseButtonJustPressed(MouseButton button) {
    bool b = glfwGetMouseButton(m_window->getWindow(), static_cast<int>(button)) == GLFW_PRESS;
    bool b2;
    auto it = m_lastMouseButtonStates.find(button);
    if (it == m_lastMouseButtonStates.end())
        b2 = false;
    else
        b2 = it->second;
    m_lastMouseButtonStates[button] = b;
    return b && !b2;
}

bool InputManager::isMouseButtonJustReleased(MouseButton button) {
    bool b = glfwGetMouseButton(m_window->getWindow(), static_cast<int>(button)) == GLFW_RELEASE;
    bool b2;
    auto it = m_lastMouseButtonStates.find(button);
    if (it == m_lastMouseButtonStates.end())
        b2 = false;
    else
        b2 = it->second;
    m_lastMouseButtonStates[button] = b;
    return b && !b2;
}

glm::vec2 InputManager::getMousePosition() {
    return m_mousePosition;
}

glm::vec2 InputManager::getMouseDelta() {
    return m_mouseDelta;
}

// void InputManager::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
//     if (action == GLFW_PRESS) {
//         InputManager::m_keyStates[static_cast<Key>(key)] = true;
//     } else if (action == GLFW_RELEASE) {
//         InputManager::m_keyStates[static_cast<Key>(key)] = false;
//     }
// }

// void InputManager::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
//     if (action == GLFW_PRESS) {
//         InputManager::m_mouseButtonStates[static_cast<MouseButton>(button)] = true;
//     } else if (action == GLFW_RELEASE) {
//         InputManager::m_mouseButtonStates[static_cast<MouseButton>(button)] = false;
//     }
// }