#pragma once

#include <cmath>

#include "Behavior.h"
#include "GameObject.h"

class Rotator : public Behavior
{
public:
    explicit Rotator(float degreesPerSecond)
        : speed(degreesPerSecond)
    {
    }

    void Start(GameObject& self) override
    {
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
        float& y = self.transform.rotationDegrees.y;

        y = std::fmod(y + speed * deltaTime, 360.0f);
    }

private:
    float speed;
};