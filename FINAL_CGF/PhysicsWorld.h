#pragma once

#include <vector>

#include "Collision.h"

class Scene;

class PhysicsWorld
{
public:
    void RebuildStaticColliders(const Scene& scene);

    bool Move(
        CapsuleCollider& capsule,
        glm::vec3& velocity,
        float deltaTime
    ) const;

private:
    bool CheckGround(
        const CapsuleCollider& capsule,
        const glm::vec3& velocity
    ) const;

    std::vector<OBB> staticColliders;
};