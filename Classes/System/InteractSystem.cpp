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
                        this->triggerAction(eid, cpInteract);
                        cpInteract.activated = true;
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
            cpRender.setFrame("gold_6.png", ecs::get<cp::Render>(eid).sprite->getParent(), 1000000);
            cpRender.manualPosMode = true;
            auto size = cpRender.sprite->getContentSize();
            cpRender.sprite->setAnchorPoint({0.5,0.5});
            cpRender.sprite->setPosition(bounds.getMidX(), bounds.getMidY());
            cpRender.sprite->setOpacity(0);
            cpRender.sprite->runAction(cc::FadeIn::create(0.3));
            cpRender.sprite->runAction(cc::Spawn::createWithTwoActions(
                cc::RotateBy::create(0.8, std::rand() % 1000 - 500),
                cc::JumpBy::create(0.8, {(float)(std::rand() % 60) - 30,
                                         (float)(std::rand() % 60) - 30}, 10, 2))
            );
            break;
    }
}

