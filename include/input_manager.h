#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include "game_window.h"
#include "input_states.h"

class InputManager
{
public:
    static void setup(GameWindow* window);
    static void update();
    static bool isKeyPressed(Key key);
    static bool isKeyReleased(Key key);
    static bool isKeyJustPressed(Key key);
    static bool isKeyJustReleased(Key key);
    static bool isMouseButtonPressed(MouseButton button);
    static bool isMouseButtonReleased(MouseButton button);
    static bool isMouseButtonJustPressed(MouseButton button);
    static bool isMouseButtonJustReleased(MouseButton button);
    static glm::vec2 getMousePosition();
    static glm::vec2 getMouseDelta();
private:
    static GameWindow* m_window;
    static glm::vec2 m_mousePosition;
    static glm::vec2 m_lastMousePosition;
    static glm::vec2 m_mouseDelta;
    static bool m_firstUpdate;
    static std::unordered_map<Key, bool> m_lastKeyStates;
    static std::unordered_map<MouseButton, bool> m_lastMouseButtonStates;
};