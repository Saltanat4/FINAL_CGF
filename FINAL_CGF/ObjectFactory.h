#pragma once

#include <memory>
#include <string>

#include "Scene.h"

class ObjectFactory
{
public:
    ObjectFactory();

    GameObject& CreateBlock(
        Scene& scene,
        const std::string& name,
        const glm::vec3& position,
        const glm::vec3& size,
        const glm::vec3& rotationDegrees = glm::vec3(0.0f)
    ) const;

    GameObject& CreateObject(
        Scene& scene,
        const std::string& name,
        std::shared_ptr<Mesh> mesh,
        const Transform& transform
    ) const;

private:
    std::shared_ptr<Mesh> cubeMesh;
};