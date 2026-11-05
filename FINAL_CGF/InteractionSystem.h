#pragma once

#include <vector>

#include "PhysicsWorld.h"

class Scene;
class GameContext;

void DispatchTriggerEvents(
    const Scene& scene,
    const std::vector<TriggerEvent>& events,
    GameContext& context
);