#include "Headers.h"

void RenderSystem::tick(double dt)
{
    //processing velocity animations
    for(auto eid : ecs.join<cp::Render, cp::Velocity>())
    {
        auto &cpRender = ecs::get<cp::Render>(eid);
        auto &cpVel = ecs::get<cp::Velocity>(eid);
        
        if (cpRender.profile != nullptr)
        {
            unsigned orientation = Dir::kNone;
            if (ecs::has<cp::Orientation>(eid))
                orientation = ecs::get<cp::Orientation>(eid).curDir;
            auto animData = cpRender.profile->getDirAnimation(orientation,
                                                              !cpVel.direction.isZero());
            if ((animData != nullptr) && (animData->key != cpRender.curAnimKey))
                cpRender.setAnimation(animData->key, -1);
        }
    }
}

void RenderSystem::animate(double dt, double tickPercent)
{
    for(auto eid : ecs.join<cp::Render, cp::Position>())
    {
        auto &cpRender = ecs::get<cp::Render>(eid);
        auto &cpPos = ecs::get<cp::Position>(eid);
        
        //position
        cocos2d::Vec2 pos(
            cpPos.pos.x * tickPercent + cpPos.last.x * (1 - tickPercent),
            cpPos.pos.y * tickPercent + cpPos.last.y * (1 - tickPercent));
        cpRender.sprite->setPosition(pos);
        
        //animation
        auto animData = cpRender.getCurAnim();
        if (animData != nullptr)
        {
            double elapsed = cpRender.elapsedTime + dt;
            auto duration = animData->duration();
            if (elapsed > duration)
            {
                if (cpRender.repeat > 0)
                    cpRender.repeat--;
                if (cpRender.repeat == 0)
                {
                    //dorepeat
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
