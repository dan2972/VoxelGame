#include "game_window.h"
#include <stdexcept>

GameWindow::GameWindow()
{
    init(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Game Window", 3, 3);
}

GameWindow::GameWindow(int width, int height, const char *title, int glMajorVersion, int glMinorVersion)
    : m_width(width), m_height(height), m_isOpen(true)
{
    init(width, height, title, glMajorVersion, glMinorVersion);
}

GameWindow::~GameWindow()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void GameWindow::update()
{
    if (m_firstUpdate)
    {
        glfwGetCursorPos(m_window, &m_mousePosition.x, &m_mousePosition.y);
        m_firstUpdate = false;
    }
    glfwPollEvents();
    glfwSwapBuffers(m_window);
    double xpos, ypos;
    glfwGetCursorPos(m_window, &xpos, &ypos);
    m_mouseDelta.x = xpos - m_mousePosition.x;
    m_mouseDelta.y = ypos - m_mousePosition.y;
    m_mousePosition.x = xpos;
    m_mousePosition.y = ypos;
}

void GameWindow::setClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

void GameWindow::setViewport(int x, int y, int width, int height)
{
    glViewport(x, y, width, height);
}

void GameWindow::setViewport(int width, int height)
{
    glViewport(0, 0, width, height);
    m_width = width;
    m_height = height;
}

void GameWindow::clear(GLbitfield mask)
{
    glClear(mask);
}

void GameWindow::setKeyCallback(void (*keyCallBack)(GLFWwindow *, int, int, int, int))
{
    glfwSetKeyCallback(m_window, keyCallBack);
}

void GameWindow::setMouseCallback(void (*mouseCallback)(GLFWwindow *, double, double))
{
    glfwSetCursorPosCallback(m_window, mouseCallback);
}

void GameWindow::setFramebufferSizeCallback(void (*framebufferSizeCallback)(GLFWwindow *, int, int))
{
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
}

bool GameWindow::isOpen() const
{
    return !glfwWindowShouldClose(m_window);
}

void GameWindow::init(int width, int height, const char *title, int glMajorVersion, int glMinorVersion)
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glMajorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glMinorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!m_window)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }

    setClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(m_window, &framebufferWidth, &framebufferHeight);
    setViewport(0, 0, framebufferWidth, framebufferHeight);
}
