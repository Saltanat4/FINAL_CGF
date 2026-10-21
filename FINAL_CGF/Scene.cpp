#include "Scene.h"

#include <stdexcept>
#include <utility>

GameObject& Scene::AddObject(
    std::unique_ptr<GameObject> object)
{
    if (!object)
    {
        throw std::invalid_argument(
            "Cannot add a null object to the scene"
        );
    }

    objects.push_back(std::move(object));

    return *objects.back();
}

const std::vector<std::unique_ptr<GameObject>>&
Scene::GetObjects() const
{
    return objects;
}

void Scene::Clear()
{
    objects.clear();
}