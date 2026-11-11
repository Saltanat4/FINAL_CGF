#pragma once

struct Material
{
    float fadeTime = 3.0f;

    static Material Default() { return Material{ 1.0f }; }
    static Material Spikes() { return Material{ .5f }; }
    static Material Key() { return Material{ 15.0f }; }
    static Material Door() { return Material{ 5.0f }; }
};