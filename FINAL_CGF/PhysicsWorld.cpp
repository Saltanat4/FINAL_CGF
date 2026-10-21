#include "PhysicsWorld.h"

#include "Scene.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

void PhysicsWorld::RebuildStaticColliders(
    const Scene& scene)
{
    std::vector<OBB> rebuilt;

    for (const auto& object : scene.GetObjects())
    {
        if (!object->collider)
        {
            continue;
        }

        const BoxCollider& collider = *object->collider;

        glm::mat4 model =
            object->transform.GetModelMatrix();

        glm::vec3 axisX = glm::vec3(model[0]);
        glm::vec3 axisY = glm::vec3(model[1]);
        glm::vec3 axisZ = glm::vec3(model[2]);

        glm::vec3 scale(
            glm::length(axisX),
            glm::length(axisY),
            glm::length(axisZ)
        );

        if (scale.x <= 0.000001f ||
            scale.y <= 0.000001f ||
            scale.z <= 0.000001f)
        {
            throw std::runtime_error(
                "Collider has zero or near-zero scale: " +
                object->name
            );
        }

        OBB box;

        box.center = glm::vec3(
            model * glm::vec4(collider.localCenter, 1.0f)
        );

        box.halfExtents =
            glm::abs(collider.halfExtents) * scale;

        box.orientation = glm::mat3(
            axisX / scale.x,
            axisY / scale.y,
            axisZ / scale.z
        );

        rebuilt.push_back(box);
    }

    staticColliders = std::move(rebuilt);
}

bool PhysicsWorld::Move(
    CapsuleCollider& capsule,
    glm::vec3& velocity,
    float deltaTime) const
{
    if (deltaTime <= 0.0f)
    {
        return false;
    }

    float movementLength =
        glm::length(velocity) * deltaTime;

    float maxStepLength = capsule.radius * 0.5f;

    int substeps = std::max(
        1,
        static_cast<int>(
            std::ceil(movementLength / maxStepLength))
    );

    float stepTime =
        deltaTime / static_cast<float>(substeps);

    for (int step = 0; step < substeps; ++step)
    {
        capsule.center += velocity * stepTime;

        for (int iteration = 0; iteration < 8; ++iteration)
        {
            bool hadCollision = false;

            for (const OBB& box : staticColliders)
            {
                Contact contact;

                if (!IntersectCapsuleOBB(
                    capsule,
                    box,
                    contact))
                {
                    continue;
                }

                hadCollision = true;

                capsule.center +=
                    contact.normal *
                    (contact.penetration + 0.0001f);

                float intoSurface = glm::dot(
                    velocity,
                    contact.normal
                );

                if (intoSurface < 0.0f)
                {
                    velocity -=
                        contact.normal * intoSurface;
                }
            }

            if (!hadCollision)
            {
                break;
            }
        }
    }

    return CheckGround(capsule, velocity);
}

bool PhysicsWorld::CheckGround(
    const CapsuleCollider& capsule,
    const glm::vec3& velocity) const
{
    CapsuleCollider probe = capsule;

    probe.center.y -= 0.02f;

    for (const OBB& box : staticColliders)
    {
        Contact contact;

        if (!IntersectCapsuleOBB(probe, box, contact))
        {
            continue;
        }

        bool walkableNormal = contact.normal.y > 0.7f;

        float movingAway = glm::dot(
            velocity,
            contact.normal
        );

        if (walkableNormal && movingAway <= 0.05f)
        {
            return true;
        }
    }

    return false;
}