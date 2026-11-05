#pragma once

#include "Interactable.h"
#include "GameContext.h"

class Spike : public Interactable
{
public:
    void OnTriggerEnter(GameObject&, GameContext& context) override
    {
        context.RequestRestart();
    }
};