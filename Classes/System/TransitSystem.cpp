#include "Headers.h"

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
        
        for(auto eid : ecs.system<cp::Warp>())
        {
            auto& cpWarp = ecs::get<cp::Warp>(eid);
            
            auto result = this->processing(cpWarp.info, cpPos, cpCol);
            if (result.first >= 1)
            {
                warpingEnter(eid2, result.second, cpWarp);
                break;
            }
        }
    }
}

void TransitSystem::warpingEnter(unsigned eid,
                                 const cocos2d::Vec2 &targetPoint,
                                 const WarpComponent &srcWarp)
{
    float duration = 0.5f;
    
    auto& render = ecs::get<cp::Render>(eid);
    ecs.del<cp::Position>(eid);
    ecs::get<cp::Velocity>(eid).reset();
    render.container->runAction(Sequence::create(
        MoveTo::create(duration, targetPoint),
        CallFunc::create(std::bind(srcWarp.onWarp)),
        NULL
    ));
    render.runAction(Sequence::create(
        TintTo::create(duration / 2, Color3B::YELLOW),
        FadeTo::create(duration / 4, 0),
        NULL
    ));
}

void TransitSystem::gateringEnter(unsigned eid, const cocos2d::Vec2& targetPoint, const GateComponent& srcGate)
{
    using namespace std::placeholders;
    float duration = 0.5f;
    
    auto& render = ecs::get<cp::Render>(eid);
    ecs.del<cp::Position>(eid);
    ecs::get<cp::Velocity>(eid).reset();
    render.container->runAction(Sequence::create(
        MoveTo::create(duration, targetPoint),
        CallFunc::create(std::bind(&TransitSystem::gateringLeave, this,
            eid, srcGate.destRoomIdx, srcGate.destGateIdx)),
        NULL
    ));
    render.runAction(Sequence::create(
        TintTo::create(duration / 2, Color3B::BLACK),
        FadeTo::create(duration / 4, 0),
        NULL
    ));
}

void TransitSystem::gateringLeave(unsigned eid, unsigned roomIdx, unsigned gateIdx)
{
    float duration = 1.0f;
    
    auto newRoom = GameCtrl::instance()->changeRoom(roomIdx, gateIdx, {eid});
    auto gateInfo = newRoom->getModel()->gates[gateIdx];
    
    auto& render = ecs::get<cp::Render>(eid);
    auto colRect = ecs::get<cp::Collision>(eid).rect;
    cocos2d::Vec2 movePos, srcPos;
    switch(gateInfo.type)
    {
        case GateInfo::Left:
            srcPos = {gateInfo.rect.getMinX() - colRect.getMinX(),
                      gateInfo.rect.getMinY() + (gateInfo.rect.size.height - colRect.size.height) / 2 - colRect.getMinY()};
            movePos = {gateInfo.rect.size.width,0};
            break;
        case GateInfo::Right:
            srcPos = {gateInfo.rect.getMaxX() - colRect.size.width - colRect.getMinX(),
                                 gateInfo.rect.getMinY() + (gateInfo.rect.size.height - colRect.size.height) / 2 - colRect.getMinY()};
            movePos = {-gateInfo.rect.size.width,0};
            break;
        case GateInfo::Up:
            srcPos = {gateInfo.rect.getMinX() + (gateInfo.rect.size.width - colRect.size.width) / 2 - colRect.getMinX(),
                                 gateInfo.rect.getMaxY() - colRect.size.height - colRect.getMinY()};
            movePos = {0,-gateInfo.rect.size.height};
            break;
        case GateInfo::Down:
            srcPos = {gateInfo.rect.getMinX() + (gateInfo.rect.size.width - colRect.size.width) / 2 - colRect.getMinX(),
                                 gateInfo.rect.getMinY() - colRect.getMinY()};
            movePos = {0,gateInfo.rect.size.height};
            break;
        default:
            Log("2: unsupported gate type detected");
            break;
    }
    auto destPos = srcPos + movePos;
    render.setPosition(srcPos);
    render.container->runAction(Sequence::create(
        MoveBy::create(duration, movePos),
        CallFunc::create([eid, destPos, roomIdx](){
            ecs::add<cp::Position>(eid, roomIdx).set(destPos);
        }),
        NULL
    ));
    render.runAction(Sequence::create(DelayTime::create(duration / 2),
                                       TintTo::create(duration / 2, Color3B::WHITE),
                                       NULL));
    render.runAction(Sequence::create(DelayTime::create(duration / 2),
                                       FadeTo::create(duration / 2, 255),
                                       NULL));
}