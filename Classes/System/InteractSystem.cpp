#include "InteractSystem.h"
#include "SysHelper.h"
#include "ModelProvider.h"

void InteractSystem::init(CollisionInfo *collision)
{
    this->collision = collision;
}

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
                        cpInteract.triggeredOnce = true;
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
        case InteractComponent::ActionType::NONE:
            break;
        case InteractComponent::ActionType::REWARD:
            //if (interact.triggeredOnce)
            //    return; //empty
            
            if (!ecs::has<cp::Gear>(eid))
                return; //no rewards
            auto& slots = ecs::get<cp::Gear>(eid);
            if (slots.size() == 0)
                return; //empty
            
            //todo using action params
            auto bounds = SysHelper::getBounds(eid);
            
            //gen rewards
            float delay = 0;
            for(auto slot : slots)
            {
                auto nid = cp::entity::genID();
                auto& cpCollec = ecs.add<cp::Collectible>(nid);
                cpCollec = slot.content->key;
                
                auto& cpRender = ecs.add<cp::Render>(nid);
                cpRender.setFrame(slot.content->spriteFrameName,
                                  ecs::get<cp::Render>(eid).sprite->getParent(),
                                  1000000);
                cpRender.manualPosMode = true;
                cpRender.sprite->setAnchorPoint({0.5,0.5});
                cpRender.sprite->setPosition(bounds.getMidX(), bounds.getMidY());
                cpRender.sprite->setOpacity(0);
                cpRender.sprite->runAction(cc::Sequence::create(
                    cc::DelayTime::create(delay),
                    cc::Spawn::create(
                        cc::FadeIn::create(0.3),
                        cc::RotateBy::create(0.8, std::rand() % 1000 - 500),
                        cc::JumpBy::create(0.8, {(float)(std::rand() % 60) - 30,
                                                 (float)(std::rand() % 60) - 30}, 10, 2),
                        NULL),
                    NULL
                ));
                cpRender.sprite->runAction(cc::Sequence::create(
                    cc::DelayTime::create(delay + 0.8),
                    cc::CallFunc::create([nid, this](){
                            auto& cpRender = ecs::get<cp::Render>(nid);
                            auto size = cpRender.sprite->getContentSize();
                            ecs.add<cp::Collision>(nid).set({0, 0, size.width, size.height},
                                                            CollisionCategory::collectible);
                            ecs.add<cp::Position>(nid).set(cpRender.sprite->getPosition() - (size / 2));
                    }),
                    NULL
                ));
                delay += 0.2;
            }
            slots.clear();
            
            break;
    }
}

