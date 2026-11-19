#pragma once

#include <string>
#include <glm/glm.hpp>

#include "Scene.h"
#include "ObjectFactory.h"
#include "GameContext.h"

struct LevelData
{
    std::string name;
    glm::vec3 playerSpawn = glm::vec3(0.0f, 1.5f, 5.0f);
    float playerYaw = -90.0f;
};

class LevelLoader
{
public:
    static LevelData Load(
        const std::string& path,
        Scene& scene,
        const ObjectFactory& factory,
        GameContext& context
    );
};