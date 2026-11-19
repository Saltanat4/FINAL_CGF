#include <glad/glad.h>

#include "Window.h"

#include <stdexcept>

namespace
{
    void FramebufferSizeCallback(
        GLFWwindow*,
        int width,
        int height)
    {
        glViewport(0, 0, width, height);
    }
}

Window::Window(int width, int height, const char* title, bool startFullscreen)
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    handle = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!handle)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }

    glfwMakeContextCurrent(handle);

    if (!gladLoadGLLoader(
        reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        glfwDestroyWindow(handle);
        handle = nullptr;

        glfwTerminate();

        throw std::runtime_error("Failed to initialize GLAD");
    }

    glfwSwapInterval(1);

    glfwSetFramebufferSizeCallback(
        handle,
        FramebufferSizeCallback);

    int framebufferWidth = 0;
    int framebufferHeight = 0;

    glfwGetFramebufferSize(
        handle,
        &framebufferWidth,
        &framebufferHeight);

    glViewport(0, 0, framebufferWidth, framebufferHeight);


    if (startFullscreen)
    {
        SetFullscreen(true);
    }
}

Window::~Window()
{
    if (handle)
    {
        glfwDestroyWindow(handle);
    }

    glfwTerminate();
}

GLFWwindow* Window::GetHandle() const
{
    return handle;
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(handle) != 0;
}

void Window::Close()
{
    glfwSetWindowShouldClose(handle, GLFW_TRUE);
}

void Window::PollEvents() const
{
    glfwPollEvents();
}

void Window::SwapBuffers() const
{
    glfwSwapBuffers(handle);
}

bool Window::IsFullscreen() const
{
    return fullscreen;
}

void Window::SetFullscreen(bool enabled)
{
    if (enabled == fullscreen)
    {
        return;
    }

    if (enabled)
    {
        glfwGetWindowPos(handle, &windowedX, &windowedY);
        glfwGetWindowSize(handle, &windowedWidth, &windowedHeight);

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        glfwSetWindowMonitor(
            handle,
            monitor,
            0, 0,
            mode->width,
            mode->height,
            mode->refreshRate
        );
    }
    else
    {
        glfwSetWindowMonitor(
            handle,
            nullptr,
            windowedX,
            windowedY,
            windowedWidth,
            windowedHeight,
            0
        );
    }

    fullscreen = enabled;

    glfwSwapInterval(1);
}

void Window::ToggleFullscreen()
{
    SetFullscreen(!fullscreen);
}