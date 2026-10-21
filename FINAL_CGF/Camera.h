#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
    explicit Camera(
        const glm::vec3& startPosition = glm::vec3(0.0f, 1.0f, 5.0f));

    void Move(const glm::vec3& localDirection, float deltaTime);
    void Rotate(float mouseDeltaX, float mouseDeltaY);

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(float aspectRatio) const;
    void SetPosition(const glm::vec3& value)
    {
        position = value;
    }

    glm::vec3 GetForward() const
    {
        return front;
    }

private:
    void UpdateVectors();

    glm::vec3 position;

    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float yaw = -90.0f;
    float pitch = -10.0f;

    float movementSpeed = 4.0f;
    float mouseSensitivity = 0.1f;

    float fieldOfView = 70.0f;
    float nearPlane = 0.1f;
    float farPlane = 200.0f;
};