#include "ObjectFactory.h"

#include "MeshFactory.h"
#include "ModelLoader.h"

#include <iostream>
#include <stdexcept>
#include <utility>
#include "Spike.h"
#include "Key.h"
#include "Door.h"
#include "Mover.h"
#include "Rotator.h"

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

std::shared_ptr<Mesh> ObjectFactory::ResolveMesh(
    const std::string& modelPath) const
{
    if (modelPath.empty())
    {
        return cubeMesh;
    }

    return LoadModel(modelPath);
}

GameObject& ObjectFactory::CreateGameObject(
    Scene& scene,
    const std::string& name,
    std::shared_ptr<Mesh> mesh,
    const glm::vec3& position,
    const glm::vec3& size,
    const glm::vec3& rotationDegrees) const
{
    if (!mesh)
    {
        throw std::invalid_argument(
            "Cannot create an object without a mesh"
        );
    }

    if (size.x <= 0.0f ||
        size.y <= 0.0f ||
        size.z <= 0.0f)
    {
        throw std::invalid_argument(
            "Object dimensions must be positive"
        );
    }

    auto object = std::make_unique<GameObject>();

    object->name = name;

    object->transform.position = position;
    object->transform.rotationDegrees = rotationDegrees;
    object->transform.scale = size;

    object->mesh = std::move(mesh);

    object->collider = BoxCollider{};
    object->material = Material::Default();


    return scene.AddObject(std::move(object));
}

GameObject& ObjectFactory::CreateBlock(
    Scene& scene,
    const std::string& name,
    const glm::vec3& position,
    const glm::vec3& size,
    const glm::vec3& rotationDegrees) const
{
    GameObject& object = CreateGameObject(
        scene,
        name,
        cubeMesh,
        position,
        size,
        rotationDegrees
    );


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
    GameObject& object = CreateGameObject(
        scene,
        name,
        ResolveMesh(modelPath),
        position,
        size,
        rotationDegrees
    );

    object.collider->type = ColliderType::Trigger;
    object.interactable = std::make_unique<Spike>();
    object.material = Material::Spikes();

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
    GameObject& object = CreateGameObject(
        scene,
        name,
        ResolveMesh(modelPath),
        position,
        size
    );

    object.collider->type = ColliderType::Trigger;
    object.interactable = std::make_unique<Key>();
    object.material = Material::Key();

    object.AddBehavior<Mover>(glm::vec3(0.0f, 0.2f, 0.0f), 0.2f, 0.1f);
    object.AddBehavior<Rotator>(90.0f);

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
    GameObject& object = CreateGameObject(
        scene,
        name,
        ResolveMesh(modelPath),
        position,
        size,
        rotationDegrees
    );

    object.collider->type = ColliderType::Trigger;
    object.interactable = std::make_unique<Door>();
    object.material = Material::Door();

    return object;
}