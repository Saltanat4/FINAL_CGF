#pragma once

#include <memory>
#include <optional>
#include <string>

#include "Transform.h"
#include "Mesh.h"
#include "Collision.h"

struct GameObject
{
    std::string name;

    Transform transform;

    std::shared_ptr<Mesh> mesh;

    std::optional<BoxCollider> collider;
};