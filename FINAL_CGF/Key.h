#pragma once

#include "Interactable.h"
#include "GameContext.h"
#include "GameObject.h"

class Key : public Interactable
{
public:
    void OnTriggerEnter(GameObject& self, GameContext& context) override
    {
        context.CollectKey();
        context.RequestDestroy(self.id);
    }
};