#include "Renderer.h"
#include <algorithm>

Renderer::Renderer()
    : shader("default.vert", "default.frag")
{
}

Renderer::~Renderer()
{
    shader.Delete();
}


void Renderer::Draw(
    const Scene& scene,
    const Camera& camera,
    float aspectRatio,
    const EchoSystem& echo,
    float time)
{
    glm::mat4 viewProjection =
        camera.GetProjectionMatrix(aspectRatio) *
        camera.GetViewMatrix();

    shader.Activate();

    glm::vec4 waveA[EchoSystem::kMaxWaves] = {};
    glm::vec4 waveB[EchoSystem::kMaxWaves] = {};

    const auto& waves = echo.GetWaves();

    const int count = std::min(
        static_cast<int>(waves.size()),
        EchoSystem::kMaxWaves);

    for (int i = 0; i < count; ++i)
    {
        waveA[i] = glm::vec4(waves[i].origin, waves[i].startTime);
        waveB[i] = glm::vec4(
            waves[i].maxRadius,
            waves[i].strength,
            0.0f,
            0.0f);
    }

    shader.SetInt("uWaveCount", count);
    shader.SetFloat("uTime", time);
    shader.SetFloat("uWaveSpeed", EchoSystem::kWaveSpeed);
    shader.SetVec4Array("uWaveA", waveA, EchoSystem::kMaxWaves);
    shader.SetVec4Array("uWaveB", waveB, EchoSystem::kMaxWaves);

    for (const auto& object : scene.GetObjects())
    {
        if (!object->mesh)
        {
            continue;
        }

        glm::mat4 model = object->transform.GetModelMatrix();

        shader.SetMat4("uModel", model);
        shader.SetMat4("uMVP", viewProjection * model);
        shader.SetVec3("uScale", object->transform.scale);
        shader.SetFloat("uFadeTime", object->material.fadeTime);

        object->mesh->Draw();
    }
}