#pragma once

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

private:
    std::vector<std::unique_ptr<GameObject>> objects;
};