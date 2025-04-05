#pragma once

#include "game_window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GameApplication
{
public:
    GameApplication();
    ~GameApplication();

    void run();

private:
    void load();
    void update();
    void render();
    void cleanup();

    void imguiInit();
    void imguiCleanup();
    void imguiNewFrame();
    void imguiEndFrame();

    GameWindow m_window;
    uint32_t m_targetTPS = 30;
};