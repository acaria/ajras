#include "RenderSystem.h"
#include "Components.h"
#include "SysHelper.h"
#include "IMapData.h"
#include "AnimationData.h"

void RenderSystem::tick(double dt)
{
    for(auto eid : context->ecs->join<cp::Render, cp::Physics, cp::Input, cp::Position>())
    {
        auto &cpRender = ecs::get<cp::Render>(eid);
        auto &cpPhy = ecs::get<cp::Physics>(eid);
        //auto &cpInput = ecs::get<cp::Input>(eid);
 
        //processing velocity animations
        if (cpRender.moveAnimation && !cpRender.busy)
        {
            Dir orientation = Dir::None;
            if (cpRender.orientationAnimation)
            {
                auto& cpPosition = ecs::get<cp::Position>(eid);
                orientation = ((cpPosition.dir != Dir::None) ? cpPosition.dir : Dir::Down);
            }
            
            cpRender.setMoveAnimation(orientation, !cpPhy.move.direction.isZero());
        }
        
        //update target mode
        //if (cpInput.actionMode == ActionMode::attack)
        //    cpRender.setMoveCategory("melee");
        //else
        cpRender.setMoveCategory("walk");
    }
}

void RenderSystem::animate(double dt, double tickPercent)
{
    for(auto eid : context->ecs->join<cp::Render, cp::Position>())
    {
        auto &cpRender = ecs::get<cp::Render>(eid);
        auto &cpPos = ecs::get<cp::Position>(eid);
        
        //position
        if (!cpRender.manualPosMode)
        {
            cc::Vec2 pos(
                cpPos.pos.x * tickPercent + cpPos.lastPos.x * (1 - tickPercent),
                cpPos.pos.y * tickPercent + cpPos.lastPos.y * (1 - tickPercent));
            cpRender.sprite->setPosition(pos);
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
