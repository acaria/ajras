#include "Headers.h"

void RenderSystem::init(RoomData *data)
{
    this->data = data;
}

void RenderSystem::tick(double dt)
{
    for(auto eid : ecs.join<cp::Render, cp::Velocity>())
    {
        auto &cpRender = ecs::get<cp::Render>(eid);
        auto &cpVel = ecs::get<cp::Velocity>(eid);
 
        //processing velocity animations
        if (!cpRender.busy && cpRender.profile != nullptr)
        {
            unsigned orientation = Dir::kNone;
            if (ecs::has<cp::Orientation>(eid))
                orientation = ecs::get<cp::Orientation>(eid).curDir;
            cpRender.setMoveAnimation(orientation, !cpVel.direction.isZero());
        }
        
        //update zorders
        if (!cpVel.velocity.isZero() && ecs::has<cp::Position, cp::Collision>(eid))
        {
            auto cpPos = ecs::get<cp::Position>(eid);
            if (cpPos.last != cpPos.pos)
            {
                auto p1 = ecs::get<cp::Collision>(eid).rect.origin;
                auto p2 = data->getModel()->getGridPos({cpPos.pos.x + p1.x, cpPos.pos.y + p1.y});
                auto zOrder = data->getModel()->getZOrder(p2);
                cpRender.setLocalZOrder(zOrder + 1);
            }
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
        cpRender.setPosition(pos);
        
        //animation
        auto animData = cpRender.getCurAnim();
        if (animData != nullptr)
        {
            cpRender.setFlippedX(animData->flipX);
            cpRender.setFlippedY(animData->flipY);
            double elapsed = cpRender.elapsedTime + dt;
            auto duration = animData->duration();
            if (elapsed > duration)
            {
                if (cpRender.repeat > 0)
                    cpRender.repeat--;
                if (cpRender.repeat == 0)
                {
                    cpRender.busy = false;
                    if (cpRender.onComplete != nullptr)
                        cpRender.onComplete(false);
                    continue;
                }
                elapsed -= duration;
            }
            cpRender.elapsedTime = elapsed;
            auto frames = animData->frameNames;
            int n = elapsed / animData->delay;
            auto curFrame = frames.at(lib::clamp(n, 0, (int)frames.size() - 1));
            cpRender.setSpriteFrame(curFrame);
        }
    }
}
