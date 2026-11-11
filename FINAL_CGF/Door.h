#pragma once

#include "Interactable.h"
#include "GameContext.h"

class Door : public Interactable
{
public:
    void OnTriggerEnter(GameObject&, GameContext& context) override
    {
        TryOpen(context);
    }

private:
    static void TryOpen(GameContext& context)
    {
        if (context.HasAllKeys())
        {
            context.RequestNextLevel();
        }
    }
};