#include "game_application.h"
#include <chrono>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

ResourceManager GameApplication::s_resourceManager;

GameApplication::GameApplication()
    : m_window(m_width, m_height, "Game Application", 3, 3)
{
}

GameApplication::~GameApplication()
{
}

void GameApplication::run()
{
    if (!load()) {
        spdlog::error("Failed to load.");
        return;
    }

    double delta = 0;
    double deltaTime = 0;
    double timer = 0;
    uint32_t ticks = 0, frames = 0;

    auto now = std::chrono::system_clock::now();
    auto lastTime = std::chrono::system_clock::now();

    while (m_window.isOpen())
    {
        now = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration<float, std::chrono::milliseconds::period>(now - lastTime).count();
        deltaTime = std::chrono::duration<double, std::chrono::seconds::period>(now - lastTime).count();
        delta += elapsed / (1000.0 / m_targetTPS);
        timer += elapsed;
        lastTime = now;
        m_gameTime.deltaTime = deltaTime;
        m_gameTime.totalTime += deltaTime;

        while(delta >= 1.0)
        {
            --delta;
            ++ticks;
            update();
        }
        ++frames;

        m_window.clear();
        m_gameTime.interpFraction = delta;
        render();

        m_window.update();

        if (timer >= 1000.0)
        {
            m_gameTime.fps = frames;
            m_gameTime.tps = ticks;
            timer -= 1000.0;
            frames = 0;
            ticks = 0;
        }
    }
    cleanup();
}

bool GameApplication::load()
{
    spdlog::set_level(spdlog::level::debug);
    m_window.setClearColor(0.47f, 0.65f, 1.0f, 1.0f);
    m_window.disableVSync();
    m_window.setKeyCallback(keyCallback);
    m_window.setFramebufferSizeCallback(framebufferSizeCallback);
    m_window.setUserPointer(this);

    m_window.enableBlend();
    m_window.setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_window.enableDepthTest();
    m_window.enableCulling();

    m_camera.updateResolution(m_width, m_height);
    
    if (!imguiInit()) {
        spdlog::error("Failed to initialize ImGui.");
        return false;
    }

    s_resourceManager.loadShader("line", "res/shaders/line_renderer.vert", "res/shaders/line_renderer.frag");
    s_resourceManager.loadShader("font", "res/shaders/font_renderer.vert", "res/shaders/font_renderer.frag");
    s_resourceManager.loadShader("font_billboard", "res/shaders/font_renderer_billboard.vert", "res/shaders/font_renderer.frag");

    s_resourceManager.addLineRenderer("default");

    auto fontRenderer = s_resourceManager.loadFontRenderer("default", "res/fonts/arial.ttf", 48);
    fontRenderer->preloadDefaultGlyphs();
    auto fontRendererBB = s_resourceManager.loadFontRenderer("default_billboard", "res/fonts/courier-mon.ttf", 48, true);
    fontRendererBB->preloadDefaultGlyphs();
    
    m_worldRenderer.loadResources();
    m_worldRenderer.getChunkMapRenderer().startBuildThread(true);

    m_world.getChunkMap().addChunkRadius({0, 0, 0}, 5);

    return true;
}

void GameApplication::update()
{
    m_world.update();
    m_worldRenderer.update();
    glm::ivec3 camChunkPos = Chunk::globalToChunkPos(m_camera.position);
    m_world.getChunkMap().addChunkRadius(camChunkPos, 6);
    m_worldRenderer.getChunkMapRenderer().queueChunkRadius(camChunkPos, 5);
}

void GameApplication::render()
{
    imguiNewFrame();

    ImGui::Begin("Debug Info");
    ImGui::Text("Delta Time: %.3fms", m_gameTime.deltaTime * 1000.0f);
    ImGui::Text("FPS: %i", m_gameTime.fps);
    ImGui::Text("TPS: %i", m_gameTime.tps);
    ImGui::Text("Total Time: %.3fs", m_gameTime.totalTime);
    ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", m_camera.position.x, m_camera.position.y, m_camera.position.z);
    if (ImGui::CollapsingHeader("Render Options")) {
        ImGui::Checkbox("Show Chunk Border", &m_worldRenderer.renderOptions.showChunkBorder);
        if (ImGui::CollapsingHeader("Light Levels")) {
            ImGui::Checkbox("Show Sun Light Levels", &m_worldRenderer.renderOptions.showSunLightLevels);
            ImGui::Checkbox("Show Block Light Levels", &m_worldRenderer.renderOptions.showBlockLightLevels);
            ImGui::SliderFloat("Radius", &m_worldRenderer.renderOptions.showLightLevelRadius, 1.0f, Chunk::CHUNK_SIZE * 2.0f);
        }
        ImGui::Checkbox("Use AO", &m_worldRenderer.renderOptions.useAO);
        ImGui::Checkbox("Use Smooth Lighting", &m_worldRenderer.renderOptions.useSmoothLighting);
        ImGui::SliderFloat("AO Factor", &m_worldRenderer.renderOptions.aoFactor, 0.0f, 1.0f);
    }
    ImGui::End();

    auto window = m_window.getWindow();

    if (m_focused) {
        m_camera.rotate(m_window.getMouseDelta().x, -m_window.getMouseDelta().y);
        m_window.disableCursor();

        float deltaTime = m_gameTime.deltaTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            m_camera.move(CameraMovement::FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            m_camera.move(CameraMovement::BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            m_camera.move(CameraMovement::LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            m_camera.move(CameraMovement::RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            m_camera.move(CameraMovement::UP, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            m_camera.move(CameraMovement::DOWN, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            m_camera.movementSpeed = 20.0f;
        else
            m_camera.movementSpeed = Camera::DEFAULT_SPEED;
    } else {
        m_window.enableCursor();
    }

    m_worldRenderer.draw(m_camera);

    imguiEndFrame();
}

void GameApplication::cleanup()
{
    imguiCleanup();
}

bool GameApplication::imguiInit() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    if (!ImGui_ImplGlfw_InitForOpenGL(m_window.getWindow(), true)) {
        spdlog::error("Failed to initialize ImGui GLFW.");
        return false;
    }

    if (!ImGui_ImplOpenGL3_Init()) {
        spdlog::error("Failed to initialize ImGui OpenGL.");
        return false;
    }

    return true;
}

void GameApplication::imguiCleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GameApplication::imguiNewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GameApplication::imguiEndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GameApplication::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) {
        return;
    }

    GameApplication* app = static_cast<GameApplication*>(glfwGetWindowUserPointer(window));
    if (app == nullptr) {
        return;
    }

    if (key == GLFW_KEY_ESCAPE) {
        app->setFocused(!app->isFocused());
    }
}

void GameApplication::framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    GameApplication* app = static_cast<GameApplication*>(glfwGetWindowUserPointer(window));
    if (app == nullptr) {
        return;
    }

    spdlog::debug("Framebuffer size changed: {}x{}", width, height);

    app->m_width = width;
    app->m_height = height;
    if (width == 0 || height == 0) {
        return;
    }
    app->m_camera.updateResolution(width, height);
    app->m_window.setViewport(0, 0, width, height);
}
