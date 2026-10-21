#pragma once

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <GLFW/glfw3.h>

class Window
{
public:
    Window(int width, int height, const char* title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    GLFWwindow* GetHandle() const;

    bool ShouldClose() const;
    void Close();

    void PollEvents() const;
    void SwapBuffers() const;

private:
    GLFWwindow* handle = nullptr;
};