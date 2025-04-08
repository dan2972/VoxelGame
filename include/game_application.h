#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "game_window.h"
#include "resource_manager.h"
#include "graphics/texture_atlas.h"
#include "graphics/font_renderer.h"

class GameApplication
{
public:
    GameApplication();
    ~GameApplication();

    void run();

    static ResourceManager& getResourceManager() { return m_resourceManager; }

private:
    bool load();
    void update();
    void render();
    void cleanup();

    bool imguiInit();
    void imguiCleanup();
    void imguiNewFrame();
    void imguiEndFrame();

    int m_width = GameWindow::DEFAULT_WIDTH;
    int m_height = GameWindow::DEFAULT_HEIGHT;

    GameWindow m_window;
    uint32_t m_targetTPS = 30;
    
    static ResourceManager m_resourceManager;

    struct GameTime
    {
        double deltaTime = 0.0; // Time since last frame in seconds
        double interpFraction = 0.0;
        double totalTime = 0.0;
        int fps = 0; // Frames per second
        int tps = 0; // Ticks per second
    };
    GameTime m_gameTime;
};