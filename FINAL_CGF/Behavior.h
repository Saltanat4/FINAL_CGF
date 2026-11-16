#pragma once

struct GameObject;
class GameContext;

class Behavior
{
public:
    virtual ~Behavior() = default;

  
    virtual void Start(GameObject& self)
    {
    }

    virtual void Update(
        GameObject& self,
        GameContext& context,
        float deltaTime)
    {
    }
};