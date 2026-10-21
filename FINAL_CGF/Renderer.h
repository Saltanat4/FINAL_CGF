#pragma once

#include "Camera.h"
#include "Scene.h"
#include "shaderClass.h"

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
        float aspectRatio
    );

private:
    Shader shader;
};