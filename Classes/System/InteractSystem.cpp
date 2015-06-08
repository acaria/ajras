#include "InteractSystem.h"
#include "SysHelper.h"

void InteractSystem::tick(double dt)
{
    for(auto eid : ecs.join<cp::Interact, cp::Render>())
    {
        auto& cpInteract = ecs::get<cp::Interact>(eid);
        if (cpInteract.triggerActivation)
        {
            cpInteract.triggerActivation = false;
            auto& cpRender = ecs::get<cp::Render>(eid);
            if (cpRender.busy || cpInteract.busy)
                continue;
            
            cpInteract.busy = true;
            if (cpInteract.activated) //turn off
            {
                cpRender.setAnimation(cpInteract.animKeyOff, 1, [&cpInteract](bool canceled){
                    if (!canceled)
                    {
                        cpInteract.activated = false;
                    }
                    cpInteract.busy = false;
                });
            }
            else //turn on
            {
                cpRender.setAnimation(cpInteract.animKeyOn, 1,
                    [this, &cpInteract, eid](bool canceled){
                    if (!canceled)
                    {
                        cpInteract.activated = true;
                    }
                    else
                    {
                        this->triggerAction(eid, cpInteract);
                    }
                    cpInteract.busy = false;
                });
            }
        }
    }
}

void InteractSystem::triggerAction(unsigned eid, InteractComponent& interact)
{
    switch(interact.action)
    {
        case InteractComponent::eAction::NONE:
            break;
        case InteractComponent::eAction::REWARD:
            //todo using action params
            auto bounds = SysHelper::getBounds(eid);
            
            //gen rewards
            auto nid = cp::entity::genID();
            
            auto& cpRender = ecs.add<cp::Render>(nid);
            cpRender.setSpriteFrame("gold_6");
            cpRender.manualPosMode = true;
            
            break;
    }
}

