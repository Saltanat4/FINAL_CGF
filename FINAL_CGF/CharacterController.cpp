#include "CharacterController.h"

CharacterController::CharacterController(
    const glm::vec3& startCenter)
{
    capsule.center = startCenter;
    capsule.radius = 0.3f;
    capsule.height = 1.8f;
}

CharacterStepResult CharacterController::FixedUpdate(
    const PhysicsWorld& world,
    const glm::vec3& moveDirection,
    bool jumpRequested,
    float deltaTime)
{
    CharacterStepResult result;

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

    if (grounded && velocity.y <= 0.0f)
    {
        coyoteTimer = coyoteTime;
    }
    else
    {
        coyoteTimer = glm::max(coyoteTimer - deltaTime, 0.0f);
    }

    if (jumpRequested)
    {
        jumpBufferTimer = jumpBufferTime;
    }
    else
    {
        jumpBufferTimer = glm::max(jumpBufferTimer - deltaTime, 0.0f);
    }

    if (jumpBufferTimer > 0.0f && (grounded || coyoteTimer > 0.0f))
    {
        velocity.y = jumpSpeed;
        grounded = false;
        coyoteTimer = 0.0f;
        jumpBufferTimer = 0.0f;
        result.jumped = true;
    }

    const bool wasGrounded = grounded;

    velocity.y += gravity * deltaTime;

    const float verticalSpeedBeforeMove = velocity.y;
    const glm::vec3 positionBefore = capsule.center;

    grounded = world.Move(
        capsule,
        velocity,
        deltaTime
    );

    if (!wasGrounded &&
        grounded &&
        verticalSpeedBeforeMove < -landingMinSpeed)
    {
        result.landed = true;
        result.landingSpeed = -verticalSpeedBeforeMove;
    }

    if (grounded)
    {
        glm::vec3 moved = capsule.center - positionBefore;
        moved.y = 0.0f;

        stepDistance += glm::length(moved);

        if (stepDistance >= strideLength)
        {
            stepDistance -= strideLength;
            result.footstep = true;
        }
    }
    else
    {
        stepDistance = 0.0f;
    }

    return result;
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

void CharacterController::Teleport(const glm::vec3& newCenter)
{
    capsule.center = newCenter;
    velocity = glm::vec3(0.0f);
    grounded = false;
    coyoteTimer = 0.0f;
    jumpBufferTimer = 0.0f;
}

const CapsuleCollider& CharacterController::GetCapsule() const
{
    return capsule;
}