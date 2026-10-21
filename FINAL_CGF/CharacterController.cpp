#include "CharacterController.h"

CharacterController::CharacterController(
    const glm::vec3& startCenter)
{
    capsule.center = startCenter;
    capsule.radius = 0.3f;
    capsule.height = 1.8f;
}

void CharacterController::FixedUpdate(
    const PhysicsWorld& world,
    const glm::vec3& moveDirection,
    bool jumpRequested,
    float deltaTime)
{
    glm::vec3 direction(
        moveDirection.x,
        0.0f,
        moveDirection.z
    );

    if (glm::dot(direction, direction) > 0.000001f)
    {
        direction = glm::normalize(direction);
    }

    velocity.x = direction.x * movementSpeed;
    velocity.z = direction.z * movementSpeed;

    if (jumpRequested && grounded)
    {
        velocity.y = jumpSpeed;
        grounded = false;
    }

    velocity.y += gravity * deltaTime;

    grounded = world.Move(
        capsule,
        velocity,
        deltaTime
    );
}

glm::vec3 CharacterController::GetEyePosition() const
{
    float eyeOffset = capsule.height * 0.5f - 0.15f;

    return capsule.center +
        glm::vec3(0.0f, eyeOffset, 0.0f);
}

bool CharacterController::IsGrounded() const
{
    return grounded;
}