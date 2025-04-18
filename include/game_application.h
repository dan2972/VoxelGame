#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "game_window.h"
#include "camera.h"
#include "resource_manager.h"
#include "graphics/gfx/texture_atlas.h"
#include "graphics/gfx/font_renderer.h"
#include "world/world.h"
#include "input_manager.h"
#include "graphics/world_renderer.h"
#include "resource_loader.h"

class GameApplication
{
public:
    GameApplication();
    ~GameApplication();

    void run();
    bool isFocused() const { return m_focused; }
    void setFocused(bool focused) { m_focused = focused; }

    static ResourceManager& getResourceManager() { return s_resourceManager; }

private:
    bool load();
    void fixedUpdate();
    void unfixedUpdate();
    void render();
    void cleanup();

    bool imguiInit();
    void imguiCleanup();
    void imguiNewFrame();
    void imguiEndFrame();

    int m_width = GameWindow::DEFAULT_WIDTH;
    int m_height = GameWindow::DEFAULT_HEIGHT;
    int m_fbWidth = GameWindow::DEFAULT_WIDTH;
    int m_fbHeight = GameWindow::DEFAULT_HEIGHT;

    GameWindow m_window;
    uint32_t m_targetTPS = 30;
    
    static ResourceManager s_resourceManager;
    ResourceLoader m_resourceLoader{&s_resourceManager};

    Camera m_camera;

    bool m_focused = true;

    struct GameTime
    {
        double deltaTime = 0.0; // Time since last frame in seconds
        double interpFraction = 0.0;
        double totalTime = 0.0;
        int fps = 0; // Frames per second
        int tps = 0; // Ticks per second
    };
    GameTime m_gameTime;

    World m_world;
    WorldRenderer m_worldRenderer{&m_world.getChunkMap(), &s_resourceManager};

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};