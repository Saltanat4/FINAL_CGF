#include "ObjectFactory.h"

#include "MeshFactory.h"
#include "ModelLoader.h"

#include <iostream>
#include <stdexcept>
#include <utility>
#include "Spike.h"
#include "Key.h"
#include "Door.h"

ObjectFactory::ObjectFactory()
    : cubeMesh(MeshFactory::CreateCube())
{
}

std::shared_ptr<Mesh> ObjectFactory::LoadModel(
    const std::string& path) const
{
    auto it = modelCache.find(path);

    if (it != modelCache.end())
    {
        return it->second;
    }

    std::shared_ptr<Mesh> mesh;

    try
    {
        mesh = ModelLoader::Load(path);
    }
    catch (const std::exception& error)
    {
        std::cerr << "Model load failed: " << error.what()
            << " (using cube)\n";

        mesh = cubeMesh;
    }

    modelCache[path] = mesh;

    return mesh;
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
    object.material = Material::Default();

    return object;
}

GameObject& ObjectFactory::CreateSpikes(
    Scene& scene,
    const std::string& name,
    const glm::vec3& position,
    const glm::vec3& size,
    const glm::vec3& rotationDegrees,
    const std::string& modelPath) const
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
    object.material = Material::Spikes();

    if (!modelPath.empty())
    {
        object.mesh = LoadModel(modelPath);
    }

    return object;
}

GameObject& ObjectFactory::CreateKey(
    Scene& scene,
    GameContext& context,
    const std::string& name,
    const glm::vec3& position,
    const glm::vec3& size,
    const std::string& modelPath) const
{
    GameObject& object = CreateBlock(
        scene,
        name,
        position,
        size
    );

    object.collider->type = ColliderType::Trigger;
    object.interactable = std::make_unique<Key>();
    object.material = Material::Key();

    if (!modelPath.empty())
    {
        object.mesh = LoadModel(modelPath);
    }

    context.AddKeyToLevel();

    return object;
}

GameObject& ObjectFactory::CreateDoor(
    Scene& scene,
    const std::string& name,
    const glm::vec3& position,
    const glm::vec3& size,
    const glm::vec3& rotationDegrees,
    const std::string& modelPath) const
{
    GameObject& object = CreateBlock(
        scene,
        name,
        position,
        size,
        rotationDegrees
    );

    object.collider->type = ColliderType::Trigger;
    object.interactable = std::make_unique<Door>();
    object.material = Material::Door();

    if (!modelPath.empty())
    {
        object.mesh = LoadModel(modelPath);
    }

    return object;
}