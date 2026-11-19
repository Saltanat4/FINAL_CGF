#pragma once

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <GLFW/glfw3.h>

class Window
{
public:
    Window(int width, int height, const char* title, bool fullscreen = false);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    GLFWwindow* GetHandle() const;

    bool ShouldClose() const;
    void Close();

    void PollEvents() const;
    void SwapBuffers() const;

    bool IsFullscreen() const;
    void SetFullscreen(bool enabled);
    void ToggleFullscreen();

private:
    GLFWwindow* handle = nullptr;

    bool fullscreen = false;

    int windowedX = 100;
    int windowedY = 100;
    int windowedWidth = 1280;
    int windowedHeight = 720;
};