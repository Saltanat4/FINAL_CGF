#include "ObjectFactory.h"

#include "MeshFactory.h"

#include <stdexcept>
#include <utility>
#include "Spike.h"

ObjectFactory::ObjectFactory()
    : cubeMesh(MeshFactory::CreateCube())
{
}

GameObject& ObjectFactory::CreateBlock(
    Scene& scene,
    const std::string& name,
    const glm::vec3& position,
    const glm::vec3& size,
    const glm::vec3& rotationDegrees) const
{
    if (size.x <= 0.0f ||
        size.y <= 0.0f ||
        size.z <= 0.0f)
    {
        throw std::invalid_argument(
            "Block dimensions must be positive"
        );
    }

    Transform transform;

    transform.position = position;
    transform.rotationDegrees = rotationDegrees;
    transform.scale = size;

    GameObject& object = CreateObject(
        scene,
        name,
        cubeMesh,
        transform
    );

    object.collider = BoxCollider{};

    return object;
}

GameObject& ObjectFactory::CreateObject(
    Scene& scene,
    const std::string& name,
    std::shared_ptr<Mesh> mesh,
    const Transform& transform) const
{
    if (!mesh)
    {
        throw std::invalid_argument(
            "Cannot create an object without a mesh"
        );
    }

    auto object = std::make_unique<GameObject>();

    object->name = name;
    object->transform = transform;
    object->mesh = std::move(mesh);

    return scene.AddObject(std::move(object));
}

GameObject& ObjectFactory::CreateSpikes(
    Scene& scene,
    const std::string& name,
    const glm::vec3& position,
    const glm::vec3& size,
    const glm::vec3& rotationDegrees) const
{
    GameObject& object = CreateBlock(
        scene,
        name,
        position,
        size,
        rotationDegrees
    );

    object.collider->type = ColliderType::Trigger;
    object.interactable = std::make_unique<Spike>();

    return object;
}