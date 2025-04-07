#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "game_window.h"
#include "resource_manager.h"
#include "graphics/texture_atlas.h"

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
    gfx::TextureAtlas<std::string> m_atlas{{.internalFilter = GL_NEAREST}};
    int m_key = 1;
    
    static ResourceManager m_resourceManager;
};