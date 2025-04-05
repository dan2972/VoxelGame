#include "game_application.h"
#include <chrono>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

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
}

void GameApplication::update()
{
    // Update game logic
}

void GameApplication::render()
{
    // Render the game
}

void GameApplication::cleanup()
{
}
