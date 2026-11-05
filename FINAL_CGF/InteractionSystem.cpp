#include "InteractionSystem.h"

#include "Scene.h"
#include "GameContext.h"
#include "Interactable.h"

void DispatchTriggerEvents(
    const Scene& scene,
    const std::vector<TriggerEvent>& events,
    GameContext& context)
{
    for (const TriggerEvent& event : events)
    {
        GameObject* object = scene.FindById(event.objectId);

        if (!object || !object->interactable)
        {
            continue;
        }

        switch (event.kind)
        {
        case TriggerEvent::Kind::Enter:
            object->interactable->OnTriggerEnter(*object, context);
            break;

        case TriggerEvent::Kind::Stay:
            object->interactable->OnTriggerStay(*object, context);
            break;

        case TriggerEvent::Kind::Exit:
            object->interactable->OnTriggerExit(*object, context);
            break;
        }
    }
}