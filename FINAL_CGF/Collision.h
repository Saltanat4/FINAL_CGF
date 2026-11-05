#pragma once

#include <glm/glm.hpp>

enum class ColliderType
{
    Solid,
    Trigger
};

struct BoxCollider
{
    glm::vec3 localCenter = glm::vec3(0.0f);

    glm::vec3 halfExtents = glm::vec3(0.5f);

    ColliderType type = ColliderType::Solid;

    bool isStatic = true;
};

struct OBB
{
    glm::vec3 center = glm::vec3(0.0f);

    glm::vec3 halfExtents = glm::vec3(0.5f);

    glm::mat3 orientation = glm::mat3(1.0f);
};

struct CapsuleCollider
{
    glm::vec3 center = glm::vec3(0.0f);

    float radius = 0.3f;

    float height = 1.8f;
};

struct Contact
{
    glm::vec3 normal = glm::vec3(0.0f);

    float penetration = 0.0f;
};

bool IntersectCapsuleOBB(
    const CapsuleCollider& capsule,
    const OBB& box,
    Contact& contact
);