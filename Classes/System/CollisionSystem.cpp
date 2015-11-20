#include "CollisionSystem.h"
#include "SysHelper.h"
#include "ModelProvider.h"
#include "IMapData.h"

CollisionSystem::~CollisionSystem()
{
}

void CollisionSystem::init(IMapData* data)
{
    this->collisionData = data->getCol();
}

void CollisionSystem::tick(double dt)
{
    for(auto eid : ecs.join<cp::Render, cp::Physics, cp::Position, cp::AI>())
    {
        auto& cpRender = ecs::get<cp::Render>(eid);
        
        if (cpRender.manualPosMode) continue;
        
        auto& cpPosition = ecs::get<cp::Position>(eid);
        auto& cpCollision = ecs::get<cp::Physics>(eid);

        cpCollision.collisionState = PhysicsComponent::NONE;
        
        if (cpCollision.move.speed == 0)
            continue;
        
        cocos2d::Rect bounds = SysHelper::getBounds(cpPosition, cpCollision);
        
        //check room limits
        if (bounds.origin.x < 0)
        {
            bounds.origin.x = 0;
            cpPosition.pos.x = - cpCollision.shape.origin.x;
        }
        
        if (bounds.origin.y < 0)
        {
            bounds.origin.y = 0;
            cpPosition.pos.y = -cpCollision.shape.origin.y;
        }
        //^^^
        //todo right? top?
        
        //----------------
        //check room blocks
        
        cc::Rect lastBounds = SysHelper::getLastBounds(cpPosition, cpCollision);
        
        if (this->collisionData->checkCollisionRect(lib::getUnion(bounds, lastBounds),
                                                    cpCollision.category))
        {
            cpCollision.collisionState = PhysicsComponent::DECOR;
            bounds.origin = this->collisionData->getCollisionPos(
                    bounds, lastBounds, cpCollision.category);
            cpPosition.pos = bounds.origin - cpCollision.shape.origin;
        }
        
        //check room objects
        for(auto oid : ecs.join<cp::Render, cp::Physics, cp::Position>())
        {
            if (oid != eid)
            {
                auto& cpPosition2 = ecs::get<cp::Position>(oid);
                auto& cpCollision2 = ecs::get<cp::Physics>(oid);
                
                cocos2d::Rect bounds2 = SysHelper::getBounds(cpPosition2, cpCollision2);
                if (bounds2.intersectsRect(bounds))
                {
                    if (cpCollision2.category == def::collision::Cat::collectible) //collectible
                    {
                        if (ecs::has<cp::Gear>(eid) && ecs::has<cp::Collectible>(oid))
                        {
                            auto collectible = ModelProvider::instance()->collectible.get(ecs::get<cp::Collectible>(oid));
                            auto& cpGear = ecs::get<cp::Gear>(eid);
                            if (SlotData::checkFreeSlot(cpGear, collectible))
                            {
                                SlotData::addCollectible(cpGear, collectible);
                                ecs::get<cp::Render>(oid).sprite->removeFromParent();
                                cp::entity::remove(oid, ecs.getID());
                                this->onGearChanged(eid, cpGear);
                            }
                        }
                    }
                    else //obstacle
                    {
                        cpCollision.collisionState = PhysicsComponent::OBJECT;
                        
                        auto diff = slide(bounds, bounds2);
                        cpPosition.pos += diff;
                        
                        if (ecs::get<cp::Physics>(oid).move.speed > 0)
                        {
                            cpPosition2.pos -= diff;
                        }

                    }
                }
            }
        }
    }
}

lib::Box CollisionSystem::bounce(const PositionComponent &cpPos,
                                 const PhysicsComponent &cpCol,
                                 const cocos2d::Rect& target)
{
    cocos2d::Vec2 pRes = cpPos.lastPos;
    auto b1 = lib::Box(cpPos.lastPos.x + cpCol.shape.getMinX(),
                       cpPos.lastPos.y + cpCol.shape.getMinY(),
                       cpCol.shape.size.width, cpCol.shape.size.height,
                       cpPos.pos.x - cpPos.lastPos.x,
                       cpPos.pos.y - cpPos.lastPos.y);
    auto b2 = lib::Box(target, {0, 0});
    
    float nX, nY;
    auto ct = lib::SweptAABB(b1, b2, nX, nY);
    auto translation = cc::Vec2(0,0);
    if (ct < 1.0f)
    {
        translation = {b1.vx * ct, b1.vy * ct};
        auto rt = 1.f - ct;
        b1.vx *= rt;
        b1.vy *= rt;
        if (fabs(nX) > 0.0001f) b1.vx = -b1.vx;
        if (fabs(nY) > 0.0001f) b1.vy = -b1.vy;
        translation += {b1.vx * rt, b1.vy * rt};
    }
    
    pRes += translation;
    return lib::Box(pRes.x, pRes.y, b1.w, b1.h, b1.vx, b1.vy);
}

cc::Vec2 CollisionSystem::slide(const cc::Rect& src,
                                const cc::Rect& target)
{
    cc::Rect intersect = {
        MAX(src.getMinX(), target.getMinX()),
        MAX(src.getMinY(), target.getMinY()),
        MIN(src.getMaxX(), target.getMaxX()) - MAX(src.getMinX(), target.getMinX()),
        MIN(src.getMaxY(), target.getMaxY()) - MAX(src.getMinY(), target.getMinY())
    };
    
    cocos2d::Vec2 result;
    if (intersect.size.width > intersect.size.height) // ySlide
    {
        if (intersect.getMinY() > src.getMinY())
            result.y = -intersect.size.height;
        else
            result.y = intersect.size.height;
    }
    else //xSlide
    {
        if (intersect.getMinX() > src.getMinX())
            result.x = -intersect.size.width;
        else
            result.x = intersect.size.width;
    }
    
    return result;
}