#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include "Transform.h"
#include "Mesh.h"
#include "Collision.h"
#include "Interactable.h"
#include "Material.h"

struct GameObject
{
    std::uint32_t id = 0;

    std::string name;

    Transform transform;

    std::shared_ptr<Mesh> mesh;

    std::optional<BoxCollider> collider;

    std::unique_ptr<Interactable> interactable;

    Material material;
};