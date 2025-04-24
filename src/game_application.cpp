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

#include "utils/algorithms.h"
#include "utils/geometry.h"

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
            fixedUpdate();
        }
        ++frames;

        unfixedUpdate();
        m_window.clear();
        m_gameTime.interpFraction = delta;
        render();

        m_window.update();
        InputManager::update();

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
    InputManager::setup(&m_window);
    m_window.setClearColor(0.47f, 0.65f, 1.0f, 1.0f);
    m_window.disableVSync();
    m_window.setFramebufferSizeCallback(framebufferSizeCallback);
    m_window.setUserPointer(this);
    m_window.getFramebufferSize(m_fbWidth, m_fbHeight);
    m_window.setViewport(0, 0, m_fbWidth, m_fbHeight);

    m_window.enableBlend();
    m_window.setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_window.enableCulling();

    m_camera.position = {0.0f, 10.0f, 0.0f};
    m_camera.updateResolution(m_width, m_height);
    m_camera.updateFramebufferSize(m_fbWidth, m_fbHeight);
    
    if (!imguiInit()) {
        spdlog::error("Failed to initialize ImGui.");
        return false;
    }
    
    m_resourceLoader.load({m_fbWidth, m_fbHeight});
    
    m_worldRenderer.loadResources();
    m_worldRenderer.getChunkMapRenderer().startBuildThread(true);

    m_world.getChunkMap().startBuildThread();

    return true;
}

void GameApplication::fixedUpdate()
{
}

void GameApplication::unfixedUpdate()
{
    m_world.update();
    m_worldRenderer.update();
    glm::ivec3 camChunkPos = Chunk::globalToChunkPos(m_camera.position);
    m_worldRenderer.getChunkMapRenderer().queueFrustum(m_camera.getFrustum(), camChunkPos, m_worldRenderer.renderOptions.renderDistance);
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
    ImGui::SliderFloat("Day/Night Fraction", &m_dayNightFrac, 0.0f, 1.0f);
    if (ImGui::CollapsingHeader("Render Options")) {
        ImGui::Checkbox("Show Chunk Border", &m_worldRenderer.renderOptions.showChunkBorder);
        ImGui::SliderInt("Render Distance", &m_worldRenderer.renderOptions.renderDistance, 1, 32);
        if (ImGui::CollapsingHeader("Light Levels")) {
            ImGui::Checkbox("Show Sun Light Levels", &m_worldRenderer.renderOptions.showSunLightLevels);
            ImGui::Checkbox("Show Block Light Levels", &m_worldRenderer.renderOptions.showBlockLightLevels);
            ImGui::SliderFloat("Radius", &m_worldRenderer.renderOptions.showLightLevelRadius, 1.0f, Chunk::CHUNK_SIZE * 2.0f);
        }
        ImGui::Checkbox("Freeze Frustum", &m_camera.freezeFrustum);
        ImGui::Checkbox("Use AO", &m_worldRenderer.renderOptions.useAO);
        ImGui::Checkbox("Use Smooth Lighting", &m_worldRenderer.renderOptions.useSmoothLighting);
        ImGui::SliderFloat("AO Factor", &m_worldRenderer.renderOptions.aoFactor, 0.0f, 1.0f);
    }
    ImGui::End();

    auto window = m_window.getWindow();

    if (InputManager::isKeyJustPressed(Key::Escape)) {
        m_focused = !m_focused;
    }

    if (m_focused) {
        auto mouseDelta = InputManager::getMouseDelta();
        m_camera.rotate(mouseDelta.x, -mouseDelta.y);
        m_window.disableCursor();

        float deltaTime = m_gameTime.deltaTime;
        if (InputManager::isKeyPressed(Key::W))
            m_camera.move(CameraMovement::FORWARD, deltaTime);
        if (InputManager::isKeyPressed(Key::S))
            m_camera.move(CameraMovement::BACKWARD, deltaTime);
        if (InputManager::isKeyPressed(Key::A))
            m_camera.move(CameraMovement::LEFT, deltaTime);
        if (InputManager::isKeyPressed(Key::D))
            m_camera.move(CameraMovement::RIGHT, deltaTime);
        if (InputManager::isKeyPressed(Key::Space))
            m_camera.move(CameraMovement::UP, deltaTime);
        if (InputManager::isKeyPressed(Key::LeftShift))
            m_camera.move(CameraMovement::DOWN, deltaTime);
        if (InputManager::isKeyPressed(Key::LeftControl))
            m_camera.movementSpeed = 40.0f;
        else
            m_camera.movementSpeed = Camera::DEFAULT_SPEED;
    } else {
        m_window.enableCursor();
    }

    auto renderTarget = s_resourceManager.getRenderTarget("game_target");
    renderTarget->use();
    m_window.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_window.disableDepthTest();
    auto skyShader = s_resourceManager.getShader("sky_color");
    skyShader->use();
    glm::mat4 viewProj = m_camera.getProjectionMatrix() * m_camera.getViewMatrix();
    skyShader->setMat4("uInvViewProj", glm::inverse(viewProj));
    skyShader->setFloat("uDayNightFrac", m_dayNightFrac);
    s_resourceManager.getScreenQuad("sky_quad")->draw();
    m_window.enableDepthTest();

    auto mousePos = InputManager::getMousePosition();
    auto lookPos = m_camera.rayDirFromNDC(0, 0);
    auto node = algo::voxelRayHit(m_camera.position, lookPos, [&](const glm::ivec3& pos) {
        BlockType block = m_world.getChunkMap().getBlock(pos);
        return block != BlockType::Air && block != BlockType::Water;
    }, 20.0f);
    BlockType block = m_world.getChunkMap().getBlock(node.pos);
    if (block != BlockType::Air && block != BlockType::Water) {
        m_worldRenderer.highlightVoxels({node.pos}, m_camera, m_window);
        if (InputManager::isMouseButtonJustPressed(MouseButton::Left) && m_focused) {
            m_world.getChunkMap().setBlock(node.pos, BlockType::Air);
            m_worldRenderer.getChunkMapRenderer().queueBlockUpdate(node.pos, BlockType::Air);
        } else if (InputManager::isMouseButtonJustPressed(MouseButton::Right) && m_focused) {
            m_world.getChunkMap().setBlock(node.pos + node.normal, BlockType::WoodPlanks);
            m_worldRenderer.getChunkMapRenderer().queueBlockUpdate(node.pos + node.normal, BlockType::WoodPlanks);
        }
    }
    m_worldRenderer.draw(m_camera, m_window, m_dayNightFrac);

    renderTarget->useDefault();
    m_window.clear(GL_COLOR_BUFFER_BIT);
    m_window.disableDepthTest();

    renderTarget->getTexture().use();
    auto screenQuadShader = s_resourceManager.getShader("screen_quad");
    screenQuadShader->use();
    screenQuadShader->setVec2("uResolution", m_camera.framebufferSize);
    screenQuadShader->setBool("uShowCrosshair", m_focused);
    s_resourceManager.getScreenQuad("game_quad")->draw();

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

void GameApplication::framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    GameApplication* app = static_cast<GameApplication*>(glfwGetWindowUserPointer(window));
    if (app == nullptr) {
        return;
    }

    spdlog::debug("Framebuffer size changed: {}x{}", width, height);

    if (width == 0 || height == 0) {
        return;
    }
    app->m_fbWidth = width;
    app->m_fbHeight = height;

    app->m_window.setViewport(0, 0, width, height);
    app->m_camera.updateFramebufferSize(width, height);
    auto renderTarget = s_resourceManager.getRenderTarget("game_target");
    renderTarget->setup(width, height);

    int windowWidth, windowHeight;
    app->m_window.getWindowSize(windowWidth, windowHeight);
    app->m_width = windowWidth;
    app->m_height = windowHeight;
    app->m_camera.updateResolution(windowWidth, windowHeight);
}
