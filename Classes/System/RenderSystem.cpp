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
            Dir orientation = Dir::None;
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
                auto pos = SysHelper::getBounds(eid);
                cpRender.setLocalZOrder(data->getModel()->getZOrder(pos.origin));
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
