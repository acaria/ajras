#include "Headers.h"

void GateSystem::tick(double dt)
{
    for(auto eid2 : ecs.join<cp::Position, cp::Collision>())
    {
        auto& cpPos = ecs::get<cp::Position>(eid2);
        auto& cpCol = ecs::get<cp::Collision>(eid2);
        cocos2d::Rect cplRect = {
            cpPos.pos.x + cpCol.rect.origin.x,
            cpPos.pos.y + cpCol.rect.origin.y,
            cpCol.rect.size.width,
            cpCol.rect.size.height
        };
        
        float facto = 0;
        cocos2d::Vec2 targetPoint;
        Dir targetDir;
        GateComponent* tgGate = nullptr;
        
        for(auto eid : ecs.system<cp::Gate>())
        {
            auto& cpGate = ecs::get<cp::Gate>(eid);
        
            if (!cpGate.info.rect.intersectsRect(cplRect))
                continue;
            
            auto cRect = lib::getIntersection(cpGate.info.rect, cplRect);
            if (cRect.size.width == 0 || cRect.size.height == 0)
                continue;
                
            switch(cpGate.info.type)
            {
                case GateInfo::Up:
                case GateInfo::Down:
                {
                    facto = cRect.size.height / cplRect.size.height;
                    if (cplRect.getMidX() > cpGate.info.rect.getMidX())
                    {
                        float xPos = cpGate.info.rect.getMaxX() - cpCol.rect.size.width;
                        xPos -= facto * (cpGate.info.rect.size.width - cpCol.rect.size.width) / 2;
                        cpPos.pos.x = MIN(cpPos.pos.x, xPos - cpCol.rect.getMinX());
                    }
                    else if (cplRect.getMidX() < cpGate.info.rect.getMidX())
                    {
                        float xPos = cpGate.info.rect.getMinX();
                        xPos += facto * (cpGate.info.rect.size.width - cpCol.rect.size.width) / 2;
                        cpPos.pos.x = MAX(cpPos.pos.x, xPos - cpCol.rect.getMinX());
                    }
                    targetDir = (cpGate.info.type == GateInfo::Up) ? Dir::Up : Dir::Down;
                    targetPoint = {cpPos.pos.x, (cpGate.info.type == GateInfo::Up) ?
                        cpGate.info.rect.getMaxY() - cplRect.size.height :
                        cpGate.info.rect.getMinY() };
                    break;
                }
                case GateInfo::Left:
                case GateInfo::Right:
                {
                    facto = cRect.size.width / cplRect.size.width;
                    if (cplRect.getMidY() > cpGate.info.rect.getMidY())
                    {
                        float yPos = cpGate.info.rect.getMaxY() - cpCol.rect.size.height;
                        yPos -= facto * (cpGate.info.rect.size.height - cpCol.rect.size.height) / 2;
                        cpPos.pos.y = MIN(cpPos.pos.y, yPos - cpCol.rect.getMinY());
                    }
                    else if (cplRect.getMidY() < cpGate.info.rect.getMidY())
                    {
                        float yPos = cpGate.info.rect.getMinY();
                        yPos += facto * (cpGate.info.rect.size.height - cpCol.rect.size.height) / 2;
                        cpPos.pos.y = MAX(cpPos.pos.y, yPos - cpCol.rect.getMinY());
                    }
                    targetDir = (cpGate.info.type == GateInfo::Left) ? Dir::Left : Dir::Right;
                    targetPoint = {(cpGate.info.type == GateInfo::Left) ? cpGate.info.rect.getMinX() :
                        (cpGate.info.rect.getMaxX() - cplRect.size.width), cpPos.pos.y };
                    break;
                }
                default:
                    Log("1: unsupported gate type detected");
                    break;
            } //switch
            if (facto > 0)
            {
                tgGate = &cpGate;
                break;
            }
        } //for
        
        if (facto >= 1.0 && tgGate != nullptr) //gate on
        {
            gateringEnter(eid2, targetPoint, *tgGate);
        }
    }
}

void GateSystem::gateringEnter(unsigned eid, const cocos2d::Vec2& targetPoint, const GateComponent& srcGate)
{
    using namespace std::placeholders;
    float duration = 0.5f;
    
    auto& render = ecs::get<cp::Render>(eid);
    ecs.del<cp::Position>(eid);
    ecs::get<cp::Velocity>(eid).reset();
    render.container->runAction(Sequence::create(
        MoveTo::create(duration, targetPoint),
        CallFunc::create(std::bind(&GateSystem::gateringLeave, this,
            eid, srcGate.destRoomIdx, srcGate.destGateIdx)),
        NULL
    ));
    render.runAction(FadeTo::create(duration / 2, 0));
    render.runAction(TintTo::create(duration / 2, Color3B::BLACK));
}

void GateSystem::gateringLeave(unsigned eid, unsigned roomIdx, unsigned gateIdx)
{
    float duration = 0.5f;
    
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