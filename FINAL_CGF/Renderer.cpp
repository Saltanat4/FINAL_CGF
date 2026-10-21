#include "Renderer.h"

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
    float aspectRatio)
{
    glm::mat4 viewProjection =
        camera.GetProjectionMatrix(aspectRatio) *
        camera.GetViewMatrix();

    shader.Activate();

    for (const auto& object : scene.GetObjects())
    {
        if (!object->mesh)
        {
            continue;
        }

        glm::mat4 mvp =
            viewProjection *
            object->transform.GetModelMatrix();

        shader.SetMat4("uMVP", mvp);

        object->mesh->Draw();
    }
}