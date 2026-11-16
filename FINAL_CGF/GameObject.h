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
#include "Behavior.h"

struct GameObject
{
    std::uint32_t id = 0;

    std::string name;

    Transform transform;

    std::shared_ptr<Mesh> mesh;

    std::optional<BoxCollider> collider;

    std::unique_ptr<Interactable> interactable;

    Material material;

    std::vector<std::unique_ptr<Behavior>> behaviors;

    template <typename T, typename... Args>
    T& AddBehavior(Args&&... args)
    {
        auto behavior = std::make_unique<T>(std::forward<Args>(args)...);

        T& reference = *behavior;

        behaviors.push_back(std::move(behavior));

        reference.Start(*this);

        return reference;
    }
};