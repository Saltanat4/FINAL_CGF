#include "Hud.h"

#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

Hud::Hud(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

Hud::~Hud()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Hud::Draw(int keysCollected, int keysInLevel, int framebufferHeight)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (keysInLevel > 0)
    {
        const float scale = framebufferHeight / 720.0f;

        ImGui::SetNextWindowPos(ImVec2(20.0f * scale, 20.0f * scale));
        ImGui::SetNextWindowBgAlpha(0.4f);

        const ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoInputs;

        if (ImGui::Begin("##hud", nullptr, flags))
        {
            ImGui::SetWindowFontScale(1.5f * scale);

            const bool allCollected = keysCollected >= keysInLevel;

            const ImVec4 color = allCollected
                ? ImVec4(0.4f, 1.0f, 0.4f, 1.0f)
                : ImVec4(1.0f, 0.8f, 0.2f, 1.0f);

            ImGui::TextColored(
                color, "KEYS  %d / %d", keysCollected, keysInLevel);

            const float size = 22.0f * scale;
            const float gap = 8.0f * scale;

            const ImVec2 origin = ImGui::GetCursorScreenPos();
            ImDrawList* draw = ImGui::GetWindowDrawList();

            for (int i = 0; i < keysInLevel; ++i)
            {
                const ImVec2 a(origin.x + i * (size + gap), origin.y);
                const ImVec2 b(a.x + size, a.y + size);

                if (i < keysCollected)
                {
                    draw->AddRectFilled(
                        a, b, IM_COL32(255, 200, 40, 255), 4.0f * scale);
                }
                else
                {
                    draw->AddRect(
                        a, b, IM_COL32(200, 200, 200, 200),
                        4.0f * scale, 0, 2.0f * scale);
                }
            }

            ImGui::Dummy(ImVec2(keysInLevel * (size + gap), size));
        }

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}