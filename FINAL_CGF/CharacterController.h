#pragma once

#include "Collision.h"
#include "PhysicsWorld.h"

struct CharacterStepResult
{
    bool jumped = false;
    bool landed = false;
    bool footstep = false;
    float landingSpeed = 0.0f;
};

class CharacterController
{
public:
    explicit CharacterController(
        const glm::vec3& startCenter
    );

    CharacterStepResult FixedUpdate(
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

    float stepDistance = 0.0f;
    float strideLength = 2.0f;
    float landingMinSpeed = 3.0f;

    bool grounded = false;
};