#include "UpdaterSystem.h"
#include "Components.h"
#include "IMapData.h"
#include "CmdFactory.h"
#include "GameCtrl.h"

void UpdaterSystem::tick(double dt)
{
    //update from inputs
    for(auto eid : context->ecs->join<cp::Input, cp::Position, cp::Physics>())
    {
        auto& cpInput = ecs::get<cp::Input>(eid);
        
        if (!cpInput.enabled) //inhibitor
            continue;
        
        auto& cpPhy = ecs::get<cp::Physics>(eid);
        
        auto& cpPosition = ecs::get<cp::Position>(eid);
        
        if (cpInput.goTo != nullptr)
        {
            auto bounds = SysHelper::getBounds(cpPosition, cpPhy);
            
            //debug path finding
            /*auto container = ecs::get<cp::Render>(eid).sprite->getParent();
            auto debugNode = container->getChildByTag<cc::DrawNode*>(def::debugTagDrawID);
            if (debugNode == nullptr)
            {
                debugNode = cc::DrawNode::create();
                container->addChild(debugNode, 0, def::debugTagDrawID);
            }
            
            debugNode->clear();
            debugNode->setLineWidth(1);
            
            context->data->getNav()->debugWaypoints(
                    eid, bounds, cpInput.goTo.Value, debugNode, cpPhy.category);
            */
            
            auto wayPoints = context->data->getNav()->getWaypoints({eid},
                bounds, cpInput.goTo.Value, cpPhy.category);
            CmdFactory::at(context->ecs, eid).goTo(wayPoints, 2);
            
            cpInput.goTo = nullptr;
        }
        else
        {
            if (cpInput.direction.isZero())
                cpPhy.inactiveInput();
            else
            {
                cpPhy.setInput(cpInput.direction);
            }
            if (ecs::has<cp::Orientation>(eid))
            {
                auto& cpOrientation = ecs::get<cp::Orientation>(eid);
                auto newDir = Dir::cardinalFromVec(cpInput.direction);
                if (newDir != Dir::None)
                {
                    if (cpOrientation.dir == Dir::None)
                        cpOrientation.dir = newDir;
                    else if (!newDir.contains(cpOrientation.dir))
                        cpOrientation.dir = newDir;
                }
            }
            
            //reset input direction
            cpInput.direction = cc::Vec2::ZERO;;
        }
    }
    
    //update stamina
    for(auto eid : context->ecs->join<cp::Stamina, cp::Input>())
    {
        if (!ecs::get<cp::Input>(eid).enabled)
            continue; //skip disabled
        auto& cpStamina = ecs::get<cp::Stamina>(eid);
        if (cpStamina.current < cpStamina.max)
        {
            cpStamina.current += cpStamina.recovery * dt;
            if (cpStamina.current > cpStamina.max)
                cpStamina.current = cpStamina.max;
        }
    }
    
    //update health
    for(auto eid : context->ecs->join<cp::Health, cp::Render>())
    {
        auto& cpHealth = ecs::get<cp::Health>(eid);
        auto& cpRender = ecs::get<cp::Render>(eid);
        
        if (cpHealth.damage > 0)
        {
            cpHealth.hp -= cpHealth.damage;
            cpHealth.damage = 0;
            
            if (cpHealth.hp <= 0)
            {
                if (ecs::has<cp::Input>(eid))
                    ecs::get<cp::Input>(eid).enabled = false;
                cpRender.sprite->stopAllActions();
                cpRender.sprite->setColor({255,255,255});
                cpRender.setAnimation("death", 1, [eid, this](bool cancel){
                    dispatcher->onEntityDeleted(context->ecs->getID(), eid);
                });
            }
        }
    }
    
#if ECSYSTEM_TRACE
    for(auto eid : context->ecs->system<cp::Render>())
    {
        auto& render = ecs::get<cp::Render>(eid);
        std::string info = lib::format("%u : Render p(%f,%f)(%f,%f)", eid,
            render.sprite->getPosition().x, render.sprite->getPosition().y,
            render.sprite->getDisplayedPosition().x, render.sprite->getDisplayedPosition().y);
        
        if (ecs::has<cp::Position>(eid))
        {
            auto& position = ecs::get<cp::Position>(eid);
            info += lib::format(" - Position p(%f,%f)", position.pos.x, position.pos.y);
        }
        
        GameCtrl::trace(std::to_string(eid), info);
    }
#endif
}