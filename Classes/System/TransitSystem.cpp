#include "TransitSystem.h"
#include "GateInfo.h"
#include "CoreLib.h"
#include "Components.h"
#include "RoomData.h"

std::pair<float, cc::Vec2> TransitSystem::processing(GateInfo info,
                                                     PositionComponent &cpPos,
                                                     PhysicsComponent &cpCol)
{
    float facto = 0;
    cc::Vec2 targetPoint = {0,0};
    
    cc::Rect cplRect = {
        cpPos.pos.x + cpCol.shape.origin.x,
        cpPos.pos.y + cpCol.shape.origin.y,
        cpCol.shape.size.width,
        cpCol.shape.size.height
    };
    
    if (!info.rect.intersectsRect(cplRect))
        return {facto, targetPoint};
    
    auto cRect = lib::getIntersection(info.rect, cplRect);
    if (cRect.size.width == 0 || cRect.size.height == 0)
        return {facto, targetPoint};
    
    switch(info.type)
    {
        case GateInfo::Up:
        case GateInfo::Down:
        {
            facto = cRect.size.height / cplRect.size.height;
            if (cplRect.getMidX() > info.rect.getMidX())
            {
                float xPos = info.rect.getMaxX() - cpCol.shape.size.width;
                xPos -= facto * (info.rect.size.width - cpCol.shape.size.width) / 2;
                cpPos.pos.x = MIN(cpPos.pos.x, xPos - cpCol.shape.getMinX());
            }
            else if (cplRect.getMidX() < info.rect.getMidX())
            {
                float xPos = info.rect.getMinX();
                xPos += facto * (info.rect.size.width - cpCol.shape.size.width) / 2;
                cpPos.pos.x = MAX(cpPos.pos.x, xPos - cpCol.shape.getMinX());
            }
            targetPoint = {cpPos.pos.x, (info.type == GateInfo::Up) ?
                info.rect.getMaxY() - cplRect.size.height :
                info.rect.getMinY() };
            break;
        }
        case GateInfo::Left:
        case GateInfo::Right:
        {
            facto = cRect.size.width / cplRect.size.width;
            if (cplRect.getMidY() > info.rect.getMidY())
            {
                float yPos = info.rect.getMaxY() - cpCol.shape.size.height;
                yPos -= facto * (info.rect.size.height - cpCol.shape.size.height) / 2;
                cpPos.pos.y = MIN(cpPos.pos.y, yPos - cpCol.shape.getMinY());
            }
            else if (cplRect.getMidY() < info.rect.getMidY())
            {
                float yPos = info.rect.getMinY();
                yPos += facto * (info.rect.size.height - cpCol.shape.size.height) / 2;
                cpPos.pos.y = MAX(cpPos.pos.y, yPos - cpCol.shape.getMinY());
            }
            targetPoint = {(info.type == GateInfo::Left) ? info.rect.getMinX() :
                (info.rect.getMaxX() - cplRect.size.width), cpPos.pos.y };
            break;
        }
        default:
            Log("1: unsupported gate type detected");
            break;
    } //switch
    
    return {facto, targetPoint};
}

void TransitSystem::tick(double dt)
{
    for(auto eid2 : context->ecs->join<cp::Position, cp::Physics, cp::Input>())
    {
        if (!ecs::get<cp::Input>(eid2).withCollision) //inhibit
            continue;
        auto& cpPos = ecs::get<cp::Position>(eid2);
        auto& cpPhy = ecs::get<cp::Physics>(eid2);
        
        //processing gates
        for(auto eid : context->ecs->system<cp::Gate>())
        {
            auto& cpGate = ecs::get<cp::Gate>(eid);
        
            auto result = this->processing(cpGate.info, cpPos, cpPhy);
            if (result.first >= 1)
            {
                gateringEnter(eid2, result.second, cpGate);
                break;
            }
        }
        
        //processing warps
        for(auto eid : context->ecs->system<cp::Warp>())
        {
            auto& cpWarp = ecs::get<cp::Warp>(eid);
            
            auto result = this->processing(cpWarp.info, cpPos, cpPhy);
            if (result.first >= 1)
            {
                warpingEnter(eid2, result.second, cpWarp);
                break;
            }
        }
    }
}

void TransitSystem::gateringEnter(unsigned eid, const cocos2d::Vec2& targetPoint,
                                  const GateMap& gateMap)
{
    using namespace std::placeholders;
    float duration = 0.5f;
    unsigned groupID = context->ecs->getID();
    
    auto& render = ecs::get<cp::Render>(eid);
    SysHelper::disableEntity(eid);
    dispatcher->onGateEnter(groupID, eid, gateMap);
    dispatcher->onEntityPositionChanged(groupID, eid);
    render.sprite->runAction(cc::Spawn::create(
        cc::Sequence::create(
            cc::MoveTo::create(duration, targetPoint),
            cc::CallFunc::create([this, eid, &gateMap](){
                this->dispatcher->onGateTriggered(context->ecs->getID(), eid, gateMap);
            }),
            NULL),
        cc::Sequence::create(
            cc::TintTo::create(duration / 4, cc::Color3B::BLACK),
            cc::FadeTo::create(duration / 6, 0),
            NULL),
        NULL
    ));
}

void TransitSystem::warpingEnter(unsigned eid, const cocos2d::Vec2& targetPoint,
                                 const WarpMap& warpMap)
{
    using namespace std::placeholders;
    float duration = 0.5f;
    
    auto& render = ecs::get<cp::Render>(eid);
    context->ecs->del<cp::Position>(eid);
    dispatcher->onEntityPositionChanged(context->ecs->getID(), eid);
    render.sprite->runAction(cc::Spawn::create(
        cc::Sequence::create(
            cc::MoveTo::create(duration, targetPoint),
            cc::CallFunc::create([this, eid, &warpMap](){
                this->dispatcher->onWarpTriggered(eid, warpMap);
            }),
            NULL),
        cc::Sequence::create(
            cc::TintTo::create(duration / 4, cc::Color3B::BLACK),
            cc::FadeTo::create(duration / 6, 0),
            NULL),
        NULL
    ));
}