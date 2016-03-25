#include "RenderSystem.h"
#include "Components.h"
#include "IMapData.h"
#include "AnimationData.h"
#include "GameCtrl.h"
#include "CmdFactory.h"

void RenderSystem::init()
{
    this->eventRegs.push_back(this->dispatcher->onSystemReady.registerObserver(
            [this](unsigned groupID){
        /*for(auto eid : this->context->ecs->join<cp::Light, cp::Render>())
        {
            unsigned group = this->context->ecs->getID();
            auto& cpLight = ecs::get<cp::Light>(eid);
            CmdFactory::at(group, eid).animParamTo("spotlight",
                {cpLight.current.brightness, cpLight.current.cutOffRadius},
                {cpLight.defaultRef.brightness, cpLight.defaultRef.cutOffRadius},
                0.5);
        }*/
    }));
    
    this->eventRegs.push_back(this->dispatcher->onGateEnter.registerObserver(
            [this](unsigned group, unsigned eid, GateMap  gate){
        switch(gate.cmd)
        {
            case GateMap::CmdType::CHANGE_ROOM:
            {
                auto focusID = GameCtrl::instance()->getData().getPlayerData()->getEntityFocusID();
                if (eid == focusID)
                {
                    auto& lightCfg = GameCtrl::instance()->getLight().currentConfig;
                    CmdFactory::at(group, eid).animParamTo("spotlight",
                        {lightCfg.spot.brightness, lightCfg.spot.cutOffRadius},
                        {0, 0},
                        0.5);
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }));
    
    this->eventRegs.push_back(this->dispatcher->onGateEnterAfter.registerObserver(
            [this](unsigned group, unsigned eid, GateMap  gate){
        switch(gate.cmd)
        {
            case GateMap::CmdType::CHANGE_ROOM:
            case GateMap::CmdType::ENTER_MAP:
            {
                auto focusID = GameCtrl::instance()->getData().getPlayerData()->getEntityFocusID();
                if (eid == focusID)
                {
                    auto& lightCfg = GameCtrl::instance()->getLight().currentConfig;
                    auto& defaultCfg = GameCtrl::instance()->getLight().defaultConfig;
                    CmdFactory::at(group, eid).animParamTo("spotlight",
                        {lightCfg.spot.brightness, lightCfg.spot.cutOffRadius},
                        {defaultCfg.spot.brightness, defaultCfg.spot.cutOffRadius},
                        0.5);
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }));
}

void RenderSystem::tick(double dt)
{
    for(auto eid : context->ecs->join<cp::Render, cp::Physics>())
    {
        auto &cpRender = ecs::get<cp::Render>(eid);
        auto &cpPhy = ecs::get<cp::Physics>(eid);
 
        //processing velocity animations
        if (cpRender.moveAnimation && !cpRender.busy)
        {
            Dir orientation = Dir::None;
            if (cpRender.orientationAnimation && ecs::has<cp::Orientation>(eid))
            {
                auto& cpOrientation = ecs::get<cp::Orientation>(eid);
                orientation = ((cpOrientation.dir != Dir::None) ? cpOrientation.dir : Dir::Down);
            }
            
            cpRender.setMoveAnimation(orientation, cpPhy.inputIsActive());
        }
        
        cpRender.setMoveCategory("walk");
    }
}

void RenderSystem::animate(double dt, double tickPercent)
{
    auto& lightCfg = GameCtrl::instance()->getLight().currentConfig;
    auto focusID = GameCtrl::instance()->getData().getPlayerData()->getEntityFocusID();
    
    for(auto eid : context->ecs->system<cp::Render>())
    {
        auto &cpRender = ecs::get<cp::Render>(eid);
        
        if (ecs::has<cp::Position>(eid))
        {
            auto &cpPos = ecs::get<cp::Position>(eid);
        
            cc::Vec2 pos(cpPos.pos.x * tickPercent + cpPos.lastPos.x * (1 - tickPercent),
                         cpPos.pos.y * tickPercent + cpPos.lastPos.y * (1 - tickPercent));
            
            cpRender.sprite->setPosition(pos);
        }
        
        if (eid == focusID && ecs::has<cp::Physics>(eid))
        {
            auto& shape = ecs::get<cp::Physics>(eid).shape;
            auto shapePos = cpRender.sprite->convertToWorldSpace(
                {shape.getMidX(), shape.getMidY()});
            lightCfg.spot.pos.x = shapePos.x;
            lightCfg.spot.pos.y = shapePos.y;
        }

        //animation
        auto animData = cpRender.getCurAnim();
        //CCASSERT(animData != nullptr, cpRender.curAnimKey.c_str());
        if (animData != nullptr)
        {
            cpRender.sprite->setFlippedX(animData->flipX);
            cpRender.sprite->setFlippedY(animData->flipY);
            double elapsed = cpRender.elapsedTime + dt;
            auto duration = animData->duration();
            if (elapsed > duration)
            {
                if (cpRender.repeat > 0)
                    cpRender.repeat--;
                if (cpRender.repeat == 0)
                {
                    if (cpRender.busy && cpRender.onComplete != nullptr)
                        cpRender.onComplete(false);
                    cpRender.busy = false;
                    continue;
                }
                elapsed -= duration;
            }
            cpRender.elapsedTime = elapsed;
            auto frames = animData->frameNames;
            int n = elapsed / animData->delay;
            auto curFrame = frames.at(lib::clamp(n, 0, (int)frames.size() - 1));
            cpRender.sprite->setSpriteFrame(curFrame);
        }
    }
}
