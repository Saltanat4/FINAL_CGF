#include "Hud.h"

#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>

namespace
{
    struct Glyph
    {
        char c;
        std::uint8_t rows[7];
    };

    const Glyph kGlyphs[] =
    {
        { 'K', { 0b10001, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010, 0b10001 } },
        { 'E', { 0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111 } },
        { 'Y', { 0b10001, 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00100 } },
        { 'S', { 0b01111, 0b10000, 0b10000, 0b01110, 0b00001, 0b00001, 0b11110 } },
        { '0', { 0b01110, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b01110 } },
        { '1', { 0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110 } },
        { '2', { 0b01110, 0b10001, 0b00001, 0b00010, 0b00100, 0b01000, 0b11111 } },
        { '3', { 0b11110, 0b00001, 0b00001, 0b01110, 0b00001, 0b00001, 0b11110 } },
        { '4', { 0b00010, 0b00110, 0b01010, 0b10010, 0b11111, 0b00010, 0b00010 } },
        { '5', { 0b11111, 0b10000, 0b11110, 0b00001, 0b00001, 0b10001, 0b01110 } },
        { '6', { 0b00110, 0b01000, 0b10000, 0b11110, 0b10001, 0b10001, 0b01110 } },
        { '7', { 0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b01000, 0b01000 } },
        { '8', { 0b01110, 0b10001, 0b10001, 0b01110, 0b10001, 0b10001, 0b01110 } },
        { '9', { 0b01110, 0b10001, 0b10001, 0b01111, 0b00001, 0b00010, 0b01100 } },
        { '/', { 0b00001, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b10000 } },
    };

    // X - основной цвет, H - блик, D - тень
    const char* const kKeySprite[12] =
    {
        "..XXXX..",
        ".XHHXXD.",
        ".XH..XD.",
        ".XH..XD.",
        ".XXXXXD.",
        "..XXXD..",
        "...XD...",
        "...XDXXX",
        "...XDDDD",
        "...XD...",
        "...XDXX.",
        "....DD..",
    };

    constexpr int kKeyW = 8;
    constexpr int kKeyH = 12;

    const std::uint8_t* FindGlyph(char c)
    {
        for (const Glyph& g : kGlyphs)
        {
            if (g.c == c) return g.rows;
        }

        return nullptr;
    }

    void Fill(ImDrawList* d, float x, float y, float w, float h, ImU32 color)
    {
        d->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color);
    }

    float TextWidth(const char* text, float p)
    {
        return static_cast<float>(std::strlen(text)) * 6.0f * p - p;
    }

    void DrawPixelText(
        ImDrawList* d, float x, float y, float p,
        const char* text, ImU32 color)
    {
        for (; *text; ++text)
        {
            if (const std::uint8_t* rows = FindGlyph(*text))
            {
                for (int r = 0; r < 7; ++r)
                {
                    for (int c = 0; c < 5; ++c)
                    {
                        if (rows[r] & (1 << (4 - c)))
                        {
                            Fill(d, x + c * p, y + r * p, p, p, color);
                        }
                    }
                }
            }

            x += 6.0f * p;
        }
    }

    void DrawShadowedText(
        ImDrawList* d, float x, float y, float p,
        const char* text, ImU32 color)
    {
        DrawPixelText(d, x + p, y + p, p, text, IM_COL32(0, 0, 0, 255));
        DrawPixelText(d, x, y, p, text, color);
    }

    void DrawKey(ImDrawList* d, float x, float y, float p, bool collected)
    {
        const ImU32 main = collected ? IM_COL32(255, 200, 40, 255) : IM_COL32(70, 70, 82, 255);
        const ImU32 light = collected ? IM_COL32(255, 238, 150, 255) : IM_COL32(92, 92, 105, 255);
        const ImU32 dark = collected ? IM_COL32(176, 112, 20, 255) : IM_COL32(44, 44, 54, 255);

        for (int r = 0; r < kKeyH; ++r)
        {
            for (int c = 0; c < kKeyW; ++c)
            {
                const char ch = kKeySprite[r][c];

                if (ch == '.') continue;

                const ImU32 color =
                    ch == 'H' ? light : (ch == 'D' ? dark : main);

                Fill(d, x + c * p, y + r * p, p, p, color);
            }
        }
    }

    // Рамка со ступенчатыми углами, как у пиксельных окон
    void DrawPanel(
        ImDrawList* d, float x, float y, float w, float h, float p,
        ImU32 border, ImU32 fill)
    {
        Fill(d, x + p, y, w - 2 * p, h, border);
        Fill(d, x, y + p, w, h - 2 * p, border);

        Fill(d, x + 2 * p, y + p, w - 4 * p, h - 2 * p, fill);
        Fill(d, x + p, y + 2 * p, w - 2 * p, h - 4 * p, fill);
    }
}

Hud::Hud(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

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
        ImDrawList* d = ImGui::GetForegroundDrawList();

        // Целый размер "пикселя": 3 на 720p, 4 на 1080p
        const float p = static_cast<float>(
            std::max(2, framebufferHeight / 240));

        const bool done = keysCollected >= keysInLevel;

        const ImU32 accent = done
            ? IM_COL32(110, 255, 110, 255)
            : IM_COL32(255, 200, 40, 255);

        const std::string count =
            std::to_string(keysCollected) + "/" + std::to_string(keysInLevel);

        const float margin = 6.0f * p;
        const float pad = 3.0f * p;

        const float labelW = TextWidth("KEYS", p);
        const float countW = TextWidth(count.c_str(), p);
        const float textW = labelW + 4.0f * p + countW;

        const float iconsW =
            keysInLevel * kKeyW * p + (keysInLevel - 1) * 2.0f * p;

        const float contentW = std::max(textW, iconsW);

        const float panelW = contentW + 2.0f * pad;
        const float panelH = pad + 7.0f * p + 3.0f * p + kKeyH * p + pad;

        const float x = margin;
        const float y = margin;

        DrawPanel(
            d, x, y, panelW, panelH, p,
            accent, IM_COL32(16, 16, 28, 235));

        float cx = x + pad;
        const float ty = y + pad;

        DrawShadowedText(d, cx, ty, p, "KEYS", IM_COL32(235, 235, 245, 255));

        cx += labelW + 4.0f * p;

        DrawShadowedText(d, cx, ty, p, count.c_str(), accent);

        const float iy = ty + 7.0f * p + 3.0f * p;

        for (int i = 0; i < keysInLevel; ++i)
        {
            const float ix = x + pad + i * (kKeyW * p + 2.0f * p);

            DrawKey(d, ix, iy, p, i < keysCollected);
        }
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}