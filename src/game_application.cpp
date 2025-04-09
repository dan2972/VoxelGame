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

ResourceManager GameApplication::m_resourceManager;

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
    m_window.setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    m_window.disableVSync();
    m_window.setKeyCallback(keyCallback);
    m_window.setFramebufferSizeCallback(framebufferSizeCallback);
    m_window.setUserPointer(this);
    
    if (!imguiInit()) {
        spdlog::error("Failed to initialize ImGui.");
        return false;
    }

    m_resourceManager.loadShader("line", "res/shaders/line_renderer.vert", "res/shaders/line_renderer.frag");
    m_resourceManager.loadShader("font", "res/shaders/font_renderer.vert", "res/shaders/font_renderer.frag");

    m_resourceManager.addLineRenderer("default");

    auto fontRenderer = m_resourceManager.loadFontRenderer("default", "res/fonts/arial.ttf", 48);
    fontRenderer->preloadDefaultGlyphs();
    
    m_window.enableBlend();
    m_window.setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_window.enableDepthTest();

    m_camera.updateResolution(m_width, m_height);

    return true;
}

void GameApplication::update()
{
    
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
    ImGui::End();

    auto fontRenderer = m_resourceManager.getFontRenderer("default");
    fontRenderer->beginBatch();
    fontRenderer->addText("Hello, World!", -5.0f, 0.0f, -20.0f, 0.1f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    auto lineRenderer = m_resourceManager.getLineRenderer("default");
    lineRenderer->beginBatch();
    //back
    lineRenderer->drawLine(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    lineRenderer->drawLine(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    lineRenderer->drawLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    lineRenderer->drawLine(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    //sides
    lineRenderer->drawLine(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    lineRenderer->drawLine(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    lineRenderer->drawLine(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    lineRenderer->drawLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    //front
    lineRenderer->drawLine(glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    lineRenderer->drawLine(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    lineRenderer->drawLine(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    lineRenderer->drawLine(glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

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
    } else {
        m_window.enableCursor();
    }
    
    glm::mat4 proj = m_camera.getProjectionMatrix();
    glm::mat4 view = m_camera.getViewMatrix();

    auto fontShader = m_resourceManager.getShader("font");
    fontShader->use();
    fontShader->setMat4("uProjection", proj);
    fontShader->setMat4("uView", view);
    fontShader->setMat4("uModel", glm::mat4(1.0f));
    fontRenderer->draw();

    auto lineShader = m_resourceManager.getShader("line");
    lineShader->use();
    lineShader->setMat4("uProjection", proj);
    lineShader->setMat4("uView", view);
    lineShader->setMat4("uModel", glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, -10.0f)));
    lineShader->setFloat("uLineWidth", 2.0f);
    lineShader->setVec2("uResolution", glm::vec2(m_width, m_height));
    lineRenderer->draw();

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
    app->m_camera.updateResolution(width, height);
    app->m_window.setViewport(0, 0, width, height);
}
