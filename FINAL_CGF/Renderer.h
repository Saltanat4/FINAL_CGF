#pragma once

#include "Camera.h"
#include "Scene.h"
#include "shaderClass.h"
#include "EchoSystem.h"

struct RenderSettings
{
    bool sonarEnabled = true;
    glm::vec3 lightDirection = glm::vec3(-0.4f, -1.0f, -0.3f);
};

class Renderer
{
public:
    Renderer();
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void Draw(
        const Scene& scene,
        const Camera& camera,
        float aspectRatio,
        const EchoSystem& echo,
        float time,
        const RenderSettings& settings);

private:
    Shader shader;
};