#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GameWindow
{
public:
    static const int DEFAULT_WIDTH = 800;
    static const int DEFAULT_HEIGHT = 600;

    struct MousePosition
    {
        double x = 0;
        double y = 0;
    };

    struct MouseDelta
    {
        double x = 0;
        double y = 0;
    };

    GameWindow();
    GameWindow(int width, int height, const char* title, int glMajorVersion=3, int glMinorVersion=3);
    ~GameWindow();

    void update();
    void swapBuffers() { glfwSwapBuffers(m_window); }
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
    void setBlendFunc(GLenum sfactor, GLenum dfactor) { glBlendFunc(sfactor, dfactor); }

    void enableVSync() { glfwSwapInterval(1); }
    void disableVSync() { glfwSwapInterval(0); }
    void disableCursor() { glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }
    void enableCursor() { glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }
    void setCursorPosition(double x, double y) { glfwSetCursorPos(m_window, x, y); }

    void setUserPointer(void* ptr) { glfwSetWindowUserPointer(m_window, ptr); }
    void* getUserPointer() const { return glfwGetWindowUserPointer(m_window); }

    void setKeyCallback(void(*keyCallBack)(GLFWwindow*, int, int, int, int));
    void setMouseCallback(void(*mouseCallback)(GLFWwindow*, double, double));
    void setFramebufferSizeCallback(void(*framebufferSizeCallback)(GLFWwindow*, int, int));

    void getFramebufferSize(int& width, int& height) const
    {
        glfwGetFramebufferSize(m_window, &width, &height);
    }

    void getWindowSize(int& width, int& height) const
    {
        glfwGetWindowSize(m_window, &width, &height);
    }

    const MousePosition& getMousePosition() const
    {
        return m_mousePosition;
    }

    const MouseDelta& getMouseDelta() const
    {
        return m_mouseDelta;
    }

    bool isOpen() const;

    GLFWwindow* getWindow() const { return m_window; }
private:
    void init(int width, int height, const char* title, int glMajorVersion, int glMinorVersion);

    GLFWwindow* m_window;
    int m_width;
    int m_height;
    bool m_isOpen;
    bool m_firstUpdate = true;

    MousePosition m_mousePosition;
    MouseDelta m_mouseDelta;
};