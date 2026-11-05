#pragma once

struct GameObject;
class GameContext;

class Interactable
{
public:
    virtual ~Interactable() = default;

    virtual void OnTriggerEnter(GameObject&, GameContext&) {}
    virtual void OnTriggerStay(GameObject&, GameContext&) {}
    virtual void OnTriggerExit(GameObject&, GameContext&) {}
};