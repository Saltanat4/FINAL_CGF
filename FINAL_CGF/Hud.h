#pragma once

#include <string>
#include <vector>

struct GLFWwindow;

class Hud
{
public:
    explicit Hud(GLFWwindow* window);
    ~Hud();

    Hud(const Hud&) = delete;
    Hud& operator=(const Hud&) = delete;


    void Draw(int keysCollected, int keysInLevel, int framebufferHeight);
    void SetStatus(std::vector<std::string> lines);
    void SetHelpVisible(bool visible);

private:
    std::vector<std::string> statusLines;
    bool helpVisible = false;
};
