#pragma once

#include "Camera.h"
#include "Scene.h"
#include "shaderClass.h"
#include "EchoSystem.h"

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
        float time);

private:
    Shader shader;
};