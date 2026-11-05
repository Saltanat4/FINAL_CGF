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
    object->id = nextId++;

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

GameObject* Scene::FindById(std::uint32_t id) const
{
    for (const auto& object : objects)
    {
        if (object->id == id)
        {
            return object.get();
        }
    }

    return nullptr;
}

bool Scene::RemoveById(std::uint32_t id)
{
    auto it = std::find_if(
        objects.begin(),
        objects.end(),
        [id](const std::unique_ptr<GameObject>& object)
        {
            return object->id == id;
        }
    );

    if (it == objects.end())
    {
        return false;
    }

    objects.erase(it);

    return true;
}