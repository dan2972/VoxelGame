#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "game_window.h"
#include "resource_manager.h"

class GameApplication
{
public:
    GameApplication();
    ~GameApplication();

    void run();

    static ResourceManager& getResourceManager() { return m_resourceManager; }

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
    
    static ResourceManager m_resourceManager;
};