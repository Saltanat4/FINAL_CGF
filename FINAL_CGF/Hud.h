#pragma once

struct GLFWwindow;

class Hud
{
public:
    explicit Hud(GLFWwindow* window);
    ~Hud();

    Hud(const Hud&) = delete;
    Hud& operator=(const Hud&) = delete;

    void Draw(int keysCollected, int keysInLevel, int framebufferHeight);
};