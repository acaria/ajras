#include "TransitSystem.h"
#include "GateInfo.h"
#include "Misc.h"
#include "Components.h"
#include "GameCtrl.h"
#include "RoomData.h"

std::pair<float, cc::Vec2> TransitSystem::processing(GateInfo info,
                                                     PositionComponent &cpPos,
                                                     CollisionComponent &cpCol)
{
    float facto = 0;
    cc::Vec2 targetPoint = {0,0};
    
    cc::Rect cplRect = {
        cpPos.pos.x + cpCol.rect.origin.x,
        cpPos.pos.y + cpCol.rect.origin.y,
        cpCol.rect.size.width,
        cpCol.rect.size.height
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
                float xPos = info.rect.getMaxX() - cpCol.rect.size.width;
                xPos -= facto * (info.rect.size.width - cpCol.rect.size.width) / 2;
                cpPos.pos.x = MIN(cpPos.pos.x, xPos - cpCol.rect.getMinX());
            }
            else if (cplRect.getMidX() < info.rect.getMidX())
            {
                float xPos = info.rect.getMinX();
                xPos += facto * (info.rect.size.width - cpCol.rect.size.width) / 2;
                cpPos.pos.x = MAX(cpPos.pos.x, xPos - cpCol.rect.getMinX());
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
                float yPos = info.rect.getMaxY() - cpCol.rect.size.height;
                yPos -= facto * (info.rect.size.height - cpCol.rect.size.height) / 2;
                cpPos.pos.y = MIN(cpPos.pos.y, yPos - cpCol.rect.getMinY());
            }
            else if (cplRect.getMidY() < info.rect.getMidY())
            {
                float yPos = info.rect.getMinY();
                yPos += facto * (info.rect.size.height - cpCol.rect.size.height) / 2;
                cpPos.pos.y = MAX(cpPos.pos.y, yPos - cpCol.rect.getMinY());
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
    for(auto eid2 : ecs.join<cp::Position, cp::Collision>())
    {
        auto& cpPos = ecs::get<cp::Position>(eid2);
        auto& cpCol = ecs::get<cp::Collision>(eid2);
        
        for(auto eid : ecs.system<cp::Gate>())
        {
            auto& cpGate = ecs::get<cp::Gate>(eid);
        
            auto result = this->processing(cpGate.info, cpPos, cpCol);
            if (result.first >= 1)
            {
                gateringEnter(eid2, result.second, cpGate);
                break;
            }
        }
    }
}

void TransitSystem::gateringEnter(unsigned eid,
                                  const cocos2d::Vec2& targetPoint,
                                  const GateMap& gateMap)
{
    using namespace std::placeholders;
    float duration = 0.5f;
    
    auto& render = ecs::get<cp::Render>(eid);
    ecs::get<cp::Input>(eid).forceDisable();
    ecs.del<cp::Position>(eid);
    ecs::get<cp::Velocity>(eid).reset();
    render.container->runAction(cc::Sequence::create(
        cc::MoveTo::create(duration, targetPoint),
        cc::CallFunc::create([this, eid, &gateMap](){
            this->onGateTriggered(eid, gateMap);
        }),
        NULL
    ));
    render.runAction(cc::Sequence::create(
        cc::TintTo::create(duration / 4, cc::Color3B::BLACK),
        cc::FadeTo::create(duration / 6, 0),
        NULL
    ));
}