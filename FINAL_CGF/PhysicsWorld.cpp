#include "PhysicsWorld.h"

#include "Scene.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

OBB PhysicsWorld::BuildOBB(const GameObject& object)
{
    const BoxCollider& collider = *object.collider;

    glm::mat4 model = object.transform.GetModelMatrix();

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
            object.name
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

    return box;
}

void PhysicsWorld::RebuildColliders(const Scene& scene)
{
    std::vector<Entry> rebuilt;

    for (const auto& object : scene.GetObjects())
    {
        if (!object->collider)
        {
            continue;
        }

        Entry entry;

        entry.objectId = object->id;
        entry.object = object.get();
        entry.type = object->collider->type;
        entry.isStatic = object->collider->isStatic;
        entry.box = BuildOBB(*object);

        rebuilt.push_back(entry);
    }

    colliders = std::move(rebuilt);
}

void PhysicsWorld::UpdateDynamicColliders()
{
    for (Entry& entry : colliders)
    {
        if (entry.isStatic)
        {
            continue;
        }

        if (!entry.object->collider)
        {
            continue;
        }

        entry.box = BuildOBB(*entry.object);
    }
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

            for (const Entry& entry : colliders)
            {
                if (entry.type != ColliderType::Solid)
                {
                    continue;
                }

                Contact contact;

                if (!IntersectCapsuleOBB(
                    capsule,
                    entry.box,
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

    for (const Entry& entry : colliders)
    {
        if (entry.type != ColliderType::Solid)
        {
            continue;
        }

        Contact contact;

        if (!IntersectCapsuleOBB(probe, entry.box, contact))
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

std::vector<TriggerEvent> PhysicsWorld::UpdateTriggers(
    const CapsuleCollider& capsule)
{
    std::vector<std::uint32_t> now;

    for (const Entry& entry : colliders)
    {
        if (entry.type != ColliderType::Trigger)
        {
            continue;
        }

        Contact contact;

        if (IntersectCapsuleOBB(capsule, entry.box, contact))
        {
            now.push_back(entry.objectId);
        }
    }

    std::sort(now.begin(), now.end());

    std::vector<TriggerEvent> events;

    for (std::uint32_t id : now)
    {
        bool wasInside = std::binary_search(
            overlapping.begin(),
            overlapping.end(),
            id
        );

        events.push_back({
            wasInside
                ? TriggerEvent::Kind::Stay
                : TriggerEvent::Kind::Enter,
            id
            });
    }

    for (std::uint32_t id : overlapping)
    {
        if (!std::binary_search(now.begin(), now.end(), id))
        {
            events.push_back({
                TriggerEvent::Kind::Exit,
                id
                });
        }
    }

    overlapping = std::move(now);

    return events;
}

void PhysicsWorld::ResetTriggerState()
{
    overlapping.clear();
}