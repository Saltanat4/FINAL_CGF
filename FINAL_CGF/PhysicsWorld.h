#pragma once

#include <cstdint>
#include <vector>

#include "Collision.h"

class Scene;
struct GameObject;

struct TriggerEvent
{
    enum class Kind
    {
        Enter,
        Stay,
        Exit
    };

    Kind kind = Kind::Enter;

    std::uint32_t objectId = 0;
};

class PhysicsWorld
{
public:
    void RebuildColliders(const Scene& scene);

    void UpdateDynamicColliders();

    bool Move(
        CapsuleCollider& capsule,
        glm::vec3& velocity,
        float deltaTime
    ) const;

    std::vector<TriggerEvent> UpdateTriggers(
        const CapsuleCollider& capsule
    );

    void ResetTriggerState();

private:
    struct Entry
    {
        std::uint32_t objectId = 0;

        const GameObject* object = nullptr;

        ColliderType type = ColliderType::Solid;

        bool isStatic = true;

        OBB box;
    };

    static OBB BuildOBB(const GameObject& object);

    bool CheckGround(
        const CapsuleCollider& capsule,
        const glm::vec3& velocity
    ) const;

    std::vector<Entry> colliders;

    std::vector<std::uint32_t> overlapping;
};