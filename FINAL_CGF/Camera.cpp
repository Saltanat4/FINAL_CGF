#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>

Camera::Camera(const glm::vec3& startPosition)
    : position(startPosition)
{
    UpdateVectors();
}

void Camera::Move(
    const glm::vec3& localDirection,
    float deltaTime)
{
    glm::vec3 movement =
        right * localDirection.x +
        worldUp * localDirection.y +
        front * localDirection.z;

    float lengthSquared = glm::dot(movement, movement);

    if (lengthSquared > 0.000001f)
    {
        position +=
            glm::normalize(movement) *
            movementSpeed *
            deltaTime;
    }
}

void Camera::Rotate(float mouseDeltaX, float mouseDeltaY)
{
    yaw += mouseDeltaX * mouseSensitivity;
    pitch += mouseDeltaY * mouseSensitivity;

    pitch = std::clamp(pitch, -89.0f, 89.0f);

    UpdateVectors();
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(
        position,
        position + front,
        up);
}

glm::mat4 Camera::GetProjectionMatrix(float aspectRatio) const
{
    return glm::perspective(
        glm::radians(fieldOfView),
        aspectRatio,
        nearPlane,
        farPlane);
}

void Camera::UpdateVectors()
{
    float yawRadians = glm::radians(yaw);
    float pitchRadians = glm::radians(pitch);

    front.x = std::cos(yawRadians) * std::cos(pitchRadians);
    front.y = std::sin(pitchRadians);
    front.z = std::sin(yawRadians) * std::cos(pitchRadians);

    front = glm::normalize(front);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::SetRotation(float newYaw, float newPitch)
{
    yaw = newYaw;
    pitch = std::clamp(newPitch, -89.0f, 89.0f);

    UpdateVectors();
}