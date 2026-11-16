#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "Scene.h"

class ObjectFactory
{
public:
    static constexpr const char* kSpikeModel = "models/spike.smf";
    static constexpr const char* kKeyModel = "models/key.smf";
    static constexpr const char* kDoorModel = "models/door.smf";

    ObjectFactory();


    GameObject& CreateGameObject(
        Scene& scene,
        const std::string& name,
        std::shared_ptr<Mesh> mesh,
        const glm::vec3& position,
        const glm::vec3& size,
        const glm::vec3& rotationDegrees = glm::vec3(0.0f)
    ) const;

    GameObject& CreateBlock(
        Scene& scene,
        const std::string& name,
        const glm::vec3& position,
        const glm::vec3& size,
        const glm::vec3& rotationDegrees = glm::vec3(0.0f)
    ) const;

    GameObject& CreateSpikes(
        Scene& scene,
        const std::string& name,
        const glm::vec3& position,
        const glm::vec3& size,
        const glm::vec3& rotationDegrees = glm::vec3(0.0f),
        const std::string& modelPath = kSpikeModel
    ) const;

    GameObject& CreateKey(
        Scene& scene,
        GameContext& context,
        const std::string& name,
        const glm::vec3& position,
        const glm::vec3& size = glm::vec3(0.3f, 0.6f, 0.1f),
        const std::string& modelPath = kKeyModel
    ) const;

    GameObject& CreateDoor(
        Scene& scene,
        const std::string& name,
        const glm::vec3& position,
        const glm::vec3& size,
        const glm::vec3& rotationDegrees = glm::vec3(0.0f),
        const std::string& modelPath = kDoorModel
    ) const;

    std::shared_ptr<Mesh> LoadModel(const std::string& path) const;

private:
    std::shared_ptr<Mesh> ResolveMesh(const std::string& modelPath) const;

    std::shared_ptr<Mesh> cubeMesh;

    mutable std::unordered_map<std::string, std::shared_ptr<Mesh>> modelCache;
};