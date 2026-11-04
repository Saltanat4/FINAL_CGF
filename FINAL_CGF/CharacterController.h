#pragma once

#include "Collision.h"
#include "PhysicsWorld.h"

class CharacterController
{
public:
    explicit CharacterController(
        const glm::vec3& startCenter
    );

    void FixedUpdate(
        const PhysicsWorld& world,
        const glm::vec3& moveDirection,
        bool jumpRequested,
        float deltaTime
    );

    void Teleport(const glm::vec3& newCenter);

    const CapsuleCollider& GetCapsule() const;

    glm::vec3 GetEyePosition() const;

    bool IsGrounded() const;

private:
    CapsuleCollider capsule;

    glm::vec3 velocity = glm::vec3(0.0f);

    float movementSpeed = 4.0f;
    float gravity = -20.0f;
    float jumpSpeed = 7.0f;

    bool grounded = false;
};