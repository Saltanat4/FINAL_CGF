#pragma once

#include <glm/glm.hpp>

struct Material
{
    float fadeTime = 3.0f;

    glm::vec3 color = glm::vec3(1.0f);

    static Material Default()
    {
        return Material{ 1.0f, glm::vec3(1, 1, 1) };
    }

    static Material Spikes()
    {
        return Material{ 0.3f, glm::vec3(1.0f, 0.25f, 0.25f) };
    }

    static Material Key()
    {
        return Material{ 10.0f, glm::vec3(1.0f, 0.9f, 0.2f) };
    }

    static Material Door()
    {
        return Material{ 30.0f, glm::vec3(0.3f, 1.0f, 0.45f) };
    }
};