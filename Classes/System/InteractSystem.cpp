#include "InteractSystem.h"
#include "SysHelper.h"
#include "GameCtrl.h"

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
            
            auto& cpCollec = ecs.add<cp::Collec>(nid);
            cpCollec = "treasure_cup";
            
            auto collectable = GameCtrl::instance()->getData().model.collectible.get(cpCollec);
            
            auto& cpRender = ecs.add<cp::Render>(nid);
            cpRender.setFrame(collectable.spriteFrameName,
                              ecs::get<cp::Render>(eid).sprite->getParent(),
                              1000000);
            cpRender.manualPosMode = true;
            cpRender.sprite->setAnchorPoint({0.5,0.5});
            cpRender.sprite->setPosition(bounds.getMidX(), bounds.getMidY());
            cpRender.sprite->setOpacity(0);
            cpRender.sprite->runAction(cc::Spawn::create(
                cc::FadeIn::create(0.3),
                cc::RotateBy::create(0.8, std::rand() % 1000 - 500),
                cc::JumpBy::create(0.8, {(float)(std::rand() % 60) - 30,
                                         (float)(std::rand() % 60) - 30}, 10, 2),
                NULL
            ));
            cpRender.sprite->runAction(cc::Sequence::create(
                cc::DelayTime::create(0.8),
                cc::CallFunc::create([nid, this](){
                    auto& cpRender = ecs::get<cp::Render>(nid);
                    auto size = cpRender.sprite->getContentSize();
                    ecs.add<cp::Collision>(nid).set({0, 0, size.width, size.height},
                                                    CollisionCategory::collectible);
                    ecs.add<cp::Position>(nid).set(cpRender.sprite->getPosition() - (size / 2));
                }),
                NULL
            ));
            
            break;
    }
}

