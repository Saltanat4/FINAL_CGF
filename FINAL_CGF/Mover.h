#pragma once

#include <glm/glm.hpp>

#include "Behavior.h"
#include "GameObject.h"

class Mover : public Behavior
{
public:
    Mover(
        const glm::vec3& offset,
        float unitsPerSecond,
        float waitSeconds)
        : offset(offset)
        , speed(unitsPerSecond)
        , wait(waitSeconds)
    {
    }

    void Start(GameObject& self) override
    {
        start = self.transform.position;
        end = start + offset;

        if (self.collider)
        {
            self.collider->isStatic = false;
        }
    }

    void Update(
        GameObject& self,
        GameContext&,
        float deltaTime) override
    {
        if (waitLeft > 0.0f)
        {
            waitLeft -= deltaTime;
            return;
        }

        const glm::vec3 target = goingToEnd ? end : start;

        const glm::vec3 toTarget = target - self.transform.position;
        const float distance = glm::length(toTarget);
        const float step = speed * deltaTime;

        if (distance <= step)
        {
            self.transform.position = target;
            goingToEnd = !goingToEnd;
            waitLeft = wait;
        }
        else
        {
            self.transform.position += toTarget / distance * step;
        }
    }

private:
    glm::vec3 offset;

    glm::vec3 start = glm::vec3(0.0f);
    glm::vec3 end = glm::vec3(0.0f);

    float speed;
    float wait;

    float waitLeft = 0.0f;
    bool goingToEnd = true;
};