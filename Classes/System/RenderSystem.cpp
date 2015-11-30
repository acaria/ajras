#include "RenderSystem.h"
#include "Components.h"
#include "SysHelper.h"
#include "IMapData.h"
#include "AnimationData.h"

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
            
            cpRender.setMoveAnimation(orientation, cpPhy.fInput().active);
        }
        
        cpRender.setMoveCategory("walk");
    }
}

void RenderSystem::animate(double dt, double tickPercent)
{
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
