#include "InteractSystem.h"
#include "ModelProvider.h"
#include "IMapData.h"

void InteractSystem::tick(double dt)
{
    for(auto eid : context->ecs->join<cp::Interact, cp::Render>())
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
                if (cpInteract.retriggerAble)
                {
                    if (cpInteract.repeatMode == -1)
                    {
                        cpRender.setAnimation(cpInteract.animKeyOff, -1);
                        cpRender.busy = false;
                        cpInteract.busy = false;
                        cpInteract.activated = false;
                        this->triggerAction(eid, cpInteract);
                    }
                    else
                    {
                        cpRender.setAnimation(cpInteract.animKeyOff, cpInteract.repeatMode,
                                [this, &cpInteract, eid](bool canceled){
                            if (!canceled)
                            {
                                cpInteract.activated = false;
                                this->triggerAction(eid, cpInteract);
                            }
                            cpInteract.busy = false;
                        });
                    }
                    
                }
            }
            else //turn on
            {
                if (cpInteract.repeatMode == -1)
                {
                    cpRender.setAnimation(cpInteract.animKeyOn, -1);
                    cpRender.busy = false;
                    cpInteract.activated = true;
                    cpInteract.busy = false;
                    this->triggerAction(eid, cpInteract);
                }
                else
                {
                    cpRender.setAnimation(cpInteract.animKeyOn, cpInteract.repeatMode,
                            [this, &cpInteract, eid](bool canceled){
                        if (!canceled)
                        {
                            cpInteract.activated = true;
                            this->triggerAction(eid, cpInteract);
                        }
                        cpInteract.busy = false;
                    });
                }
            }
        }
    }
}

void InteractSystem::triggerAction(unsigned eid, InteractComponent& interact)
{
    switch(lib::hash(interact.action))
    {
        case lib::hash("none"): {
            break;
        }
        case lib::hash("reward"): {
            if (!ecs::has<cp::Gear>(eid) || !interact.activated)
                return; //no rewards
            auto& gear = ecs::get<cp::Gear>(eid);
            if (gear.slots.size() == 0)
                return; //empty
            
            //todo using action params
            auto bounds = SysHelper::getBounds(eid);
            
            //gen rewards
            float delay = 0;
            for(auto slot : gear.slots)
            {
                auto nid = cp::entity::genID();
                auto& cpCollec = context->ecs->add<cp::Collectible>(nid);
                cpCollec = slot.content->key;
                
                auto& cpRender = context->ecs->add<cp::Render>(nid);
                cpRender.setFrame(slot.content->spriteFrameName,
                                  ecs::get<cp::Render>(eid).sprite->getContainer(),
                                  def::LayerType::MAIN2);
                cc::Point srcPos = {bounds.getMidX(), bounds.getMidY()};
                cpRender.sprite->setAnchorPoint({0.5,0.5});
                cpRender.sprite->setPosition(srcPos);
                cpRender.sprite->setOpacity(0);
                
                auto bList = context->data->getCol()->getNearEmptyBlocks(srcPos, 2, def::collision::Cat::walkable);
                auto destRect = bList[cc::random(0, (int)bList.size() - 1)];
                cc::Point destPos = {
                    destRect.origin.x + (cc::random(0, (int)destRect.size.width)) - srcPos.x,
                    destRect.origin.y + (cc::random(0, (int)destRect.size.height))  - srcPos.y
                };
                cpRender.sprite->runAction(cc::Sequence::create(
                    cc::DelayTime::create(delay),
                    cc::Spawn::create(
                        cc::FadeIn::create(0.3),
                        //todo rand
                        cc::RotateBy::create(0.8, 360),
                        cc::JumpBy::create(0.8, destPos, 10, 2),
                        NULL),
                    NULL
                ));
                cpRender.sprite->runAction(cc::Sequence::create(
                    cc::DelayTime::create(delay + 0.8),
                    cc::CallFunc::create([nid, this](){
                            auto& cpRender = ecs::get<cp::Render>(nid);
                            auto size = cpRender.sprite->getContentSize();
                            auto& cpPhysics = context->ecs->add<cp::Physics>(nid);
                            cpPhysics.shape = {0, 0, size.width, size.height};
                            cpPhysics.category = def::collision::Cat::collectible;
                            auto spos = cpRender.sprite->getPosition() - size / 2;
                            cpRender.sprite->setAnchorPoint({0,0});
                            cpRender.sprite->setPosition(spos);
                            context->ecs->add<cp::Position>(nid).set(spos);
                            dispatcher->onEntityAdded(context->ecs->getID(), nid);
                    }),
                    NULL
                ));
                delay += 0.2;
            }
            gear.slots.clear();
            break;
        }
        case lib::hash("light"): {
            if (!ecs::has<cp::Light>(eid))
                return; //no lights
            auto& cpLight = ecs::get<cp::Light>(eid);
            auto sprite = cpLight.halo;
            if (interact.activated)
            {
                cc::Vec2 scaleTarget = {
                    cpLight.defaultSize.width / sprite->getContentSize().width,
                    cpLight.defaultSize.height / sprite->getContentSize().height,
                };
                sprite->runAction(cc::Spawn::create(
                    cc::ActionFloat::create(def::anim::toggleLightDuration, sprite->getScaleX(), scaleTarget.x,
                        [sprite](float v){ sprite->setScaleX(v); }),
                    cc::ActionFloat::create(def::anim::toggleLightDuration, sprite->getScaleY(), scaleTarget.y,
                        [sprite](float v){ sprite->setScaleY(v); }),
                    NULL)
                );
            }
            else //disabled
            {
                sprite->runAction(cc::Spawn::create(
                    cc::ActionFloat::create(def::anim::toggleLightDuration, sprite->getScaleX(), 0,
                        [sprite](float v){ sprite->setScaleX(v); }),
                    cc::ActionFloat::create(def::anim::toggleLightDuration, sprite->getScaleY(), 0,
                        [sprite](float v){ sprite->setScaleY(v); }),
                    NULL)
                );
            }
            break;
        }
        default: {
            Log("invalid trigger action: %s", interact.action.c_str());
            break;
        }
    }
}

