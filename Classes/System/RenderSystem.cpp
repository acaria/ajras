#include "RenderSystem.h"
#include "Components.h"
#include "SysHelper.h"
#include "RoomData.h"
#include "AnimationData.h"

void RenderSystem::init(RoomData *data)
{
    this->data = data;
}

void RenderSystem::tick(double dt)
{
    for(auto eid : ecs.join<cp::Render, cp::Velocity, cp::Input>())
    {
        auto &cpRender = ecs::get<cp::Render>(eid);
        auto &cpVel = ecs::get<cp::Velocity>(eid);
        auto &cpInput = ecs::get<cp::Input>(eid);
 
        //processing velocity animations
        if (!cpRender.busy && cpRender.profile != nullptr)
        {
            Dir orientation = Dir::None;
            if (ecs::has<cp::Orientation>(eid))
            {
                auto& cpOrientation = ecs::get<cp::Orientation>(eid);
                orientation = Dir::cardinalFromVec(cpOrientation.visual);
                if (orientation == Dir::None)
                    orientation = cpOrientation.curDir;
            }
            cpRender.setMoveAnimation(orientation, !cpVel.direction.isZero());
        }
        
        //update zorders
        if (!cpVel.velocity.isZero() && ecs::has<cp::Position, cp::Collision>(eid))
        {
            auto cpPos = ecs::get<cp::Position>(eid);
            if (cpPos.last != cpPos.pos)
            {
                auto pos = SysHelper::getBounds(eid);
                cpRender.sprite->setLocalZOrder(data->getZOrder(pos.origin));
            }
        }
        
        //update target mode
        if (cpInput.actionMode == ActionMode::attack)
            cpRender.setMoveCategory("melee");
        else
            cpRender.setMoveCategory("walk");
    }
}

void RenderSystem::animate(double dt, double tickPercent)
{
    for(auto eid : ecs.join<cp::Render, cp::Position>())
    {
        auto &cpRender = ecs::get<cp::Render>(eid);
        auto &cpPos = ecs::get<cp::Position>(eid);
        
        //position
        if (!cpRender.manualPosMode)
        {
            cc::Vec2 pos(
                cpPos.pos.x * tickPercent + cpPos.last.x * (1 - tickPercent),
                cpPos.pos.y * tickPercent + cpPos.last.y * (1 - tickPercent));
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
