#include "game_application.h"
#include <chrono>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

ResourceManager GameApplication::m_resourceManager;

GameApplication::GameApplication()
    : m_window(GameWindow::DEFAULT_WIDTH, GameWindow::DEFAULT_HEIGHT, "Game Application", 3, 3)
{
}

GameApplication::~GameApplication()
{
}

void GameApplication::run()
{
    load();

    double delta = 0;
    double deltaTime = 0;
    double timer = 0;
    uint32_t ticks = 0, frames = 0;
    uint32_t fps, tps;

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

        while(delta >= 1.0)
        {
            --delta;
            ++ticks;
            update();
        }
        ++frames;

        m_window.clear();
        render();

        m_window.swapBuffers();
        m_window.pollEvents();

        if (timer >= 1000.0)
        {
            fps = frames;
            tps = ticks;
            timer -= 1000.0;
            frames = 0;
            ticks = 0;
            
            spdlog::info("FPS: {}, TPS: {}", fps, tps);
        }
    }
    cleanup();
}

void GameApplication::load()
{
    spdlog::set_level(spdlog::level::debug);
    m_window.setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    m_window.disableVSync();

    imguiInit();
}

void GameApplication::update()
{
    
}

void GameApplication::render()
{
    imguiNewFrame();

    ImGui::Begin("Hello, world!");
    ImGui::Text("This is some useful text.");
    ImGui::End();

    imguiEndFrame();
}

void GameApplication::cleanup()
{
    imguiCleanup();
}

void GameApplication::imguiInit() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui_ImplGlfw_InitForOpenGL(m_window.getWindow(), true);
    ImGui_ImplOpenGL3_Init();
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
