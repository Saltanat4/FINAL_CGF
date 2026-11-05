#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "GameObject.h"

class Scene
{
public:
    GameObject& AddObject(
        std::unique_ptr<GameObject> object
    );

    const std::vector<std::unique_ptr<GameObject>>&
        GetObjects() const;

    void Clear();
    GameObject* FindById(std::uint32_t id) const;
    bool RemoveById(std::uint32_t id);

private:
    std::vector<std::unique_ptr<GameObject>> objects;

    std::uint32_t nextId = 1;
};