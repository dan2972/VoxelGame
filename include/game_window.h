#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef void (*KeyCallback)(GLFWwindow*, int, int, int, int);
typedef void (*MouseCallback)(GLFWwindow*, double, double);
typedef void (*FramebufferSizeCallback)(GLFWwindow*, int, int);

class GameWindow
{
public:
    static const int DEFAULT_WIDTH = 800;
    static const int DEFAULT_HEIGHT = 600;

    GameWindow();
    GameWindow(int width, int height, const char* title, int glMajorVersion=3, int glMinorVersion=3);
    ~GameWindow();

    void swapBuffers();
    void pollEvents() { glfwPollEvents(); }
    void setClearColor(float r, float g, float b, float a);
    void setViewport(int x, int y, int width, int height);
    void setViewport(int width, int height);
    void clear(GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    void enableDepthTest() { glEnable(GL_DEPTH_TEST); }
    void disableDepthTest() { glDisable(GL_DEPTH_TEST); }
    void enableBlend() { glEnable(GL_BLEND); }
    void disableBlend() { glDisable(GL_BLEND); }
    void enableCulling() { glEnable(GL_CULL_FACE); }
    void disableCulling() { glDisable(GL_CULL_FACE); }
    void enableStencilTest() { glEnable(GL_STENCIL_TEST); }
    void disableStencilTest() { glDisable(GL_STENCIL_TEST); }

    void enableVSync() { glfwSwapInterval(1); }
    void disableVSync() { glfwSwapInterval(0); }
    void disableCursor() { glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }
    void enableCursor() { glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }
    void setCursorPosition(double x, double y) { glfwSetCursorPos(m_window, x, y); }

    void setKeyCallback(KeyCallback keyCallback);
    void setMouseCallback(MouseCallback mouseCallback);
    void setFramebufferSizeCallback(FramebufferSizeCallback framebufferSizeCallback);

    void getFramebufferSize(int& width, int& height) const
    {
        glfwGetFramebufferSize(m_window, &width, &height);
    }

    void getWindowSize(int& width, int& height) const
    {
        glfwGetWindowSize(m_window, &width, &height);
    }

    bool isOpen() const;
private:
    void init(int width, int height, const char* title, int glMajorVersion, int glMinorVersion);

    GLFWwindow* m_window;
    int m_width;
    int m_height;
    bool m_isOpen;
};