#include "CollisionSystem.h"
#include "SysHelper.h"
#include "ModelProvider.h"

CollisionSystem::~CollisionSystem()
{
    this->reset();
}

void CollisionSystem::reset()
{
    for(auto pair : this->grids)
    {
        if (pair.second != nullptr)
            delete pair.second;
    }
    this->grids.clear();
}

lib::v2u CollisionSystem::getGridPosIntersect(float x, float y)
{
    return { (unsigned)(x / this->blockSize.x), (unsigned)(y / this->blockSize.y) };
}

lib::v2u CollisionSystem::getGridPosIntersect(const cocos2d::Vec2& v)
{
    return this->getGridPosIntersect(v.x, v.y);
}

bool CollisionSystem::checkRoomCollision(const cocos2d::Rect &rect, CollisionCategory cat)
{
    auto moveAble = this->grids[cat];
    if (!moveAble->get(this->getGridPosIntersect({rect.getMinX(), rect.getMinY()})))
        return true;
    if (!moveAble->get(this->getGridPosIntersect({rect.getMinX(), rect.getMaxY()})))
        return true;
    if (!moveAble->get(this->getGridPosIntersect({rect.getMaxX(), rect.getMinY()})))
        return true;
    if (!moveAble->get(this->getGridPosIntersect({rect.getMaxX(), rect.getMaxY()})))
        return true;
    return false;
}

std::list<cocos2d::Rect> CollisionSystem::getRectGridCollisions(const cocos2d::Rect& rect, CollisionCategory cat)
{
    auto moveAble = this->grids[cat];
    auto res = std::list<cocos2d::Rect>();
    
    auto upLeft = this->getGridPosIntersect(rect.getMinX() - 1, rect.getMaxY() +  1);
    auto downRight = this->getGridPosIntersect(rect.getMaxX() + 1, rect.getMinY() - 1);
    
    for(unsigned x = upLeft.x; x <= downRight.x; x++)
    for(unsigned y = downRight.y; y <= upLeft.y; y++)
    {
        if (!moveAble->get({x,y}))
        {
            auto gridRect = cc::Rect(
                x * blockSize.x, y * blockSize.y, blockSize.x, blockSize.y);
            cc::Vec2 origin = {
                MAX(gridRect.getMinX(), rect.getMinX()),
                MAX(gridRect.getMinY(), rect.getMinY())
            };
            res.push_back(cocos2d::Rect(
                origin.x, origin.y,
                MIN(gridRect.getMaxX(), rect.getMaxX()) - origin.x,
                MIN(gridRect.getMaxY(), rect.getMaxY()) - origin.y
            ));
        }
    }
    
    res.sort([&rect](const cocos2d::Rect& r1, const cocos2d::Rect& r2){
        return r1.size.width * r1.size.height > r2.size.width * r2.size.height;
    });
    
    return res;
}

void CollisionSystem::init(IMapData* map)
{
    this->reset();
    
    this->blockSize = {(unsigned)map->getTileSize().width,
                       (unsigned)map->getTileSize().height};
    this->grids[CollisionCategory::walkable] = new lib::DataGrid<bool>(
            map->getGrid().width,
            map->getGrid().height);
    this->grids[CollisionCategory::flyable] = new lib::DataGrid<bool>(
            map->getGrid().width,
            map->getGrid().height);

    for(unsigned j = 0; j < map->getGrid().height; j++)
    for(unsigned i = 0; i < map->getGrid().width; i++)
    {
        auto fields = map->getGrid().get(i,j).fields;
        if (fields.find(BlockInfo::PType::collision) != fields.end())
        {
            auto category = fields[BlockInfo::PType::collision];
            if (category == "walkable")
            {
                this->grids[CollisionCategory::flyable]->get({i, j}) = true;
                this->grids[CollisionCategory::walkable]->get({i, j}) = true;
            }
            else if (category == "flyable")
                this->grids[CollisionCategory::flyable]->get({i, j}) = true;
            else
                Log("invalid collision category: %s", category.c_str());
        }
    }
}

void CollisionSystem::tick(double dt)
{
#if kDrawDebug
    for(auto eid : ecs.join<cp::Render, cp::Collision, cp::Position>())
    {
        auto& cpRender = ecs::get<cp::Render>(eid);
        auto& cpCollision = ecs::get<cp::Collision>(eid);
        cpRender.collision->setVisible(true);
        cpRender.collision->setColor(cc::Color3B::GREEN);
        cpRender.collision->setPosition(cpCollision.rect.origin.x,
                                        cpCollision.rect.origin.y);
        cpRender.collision->setScale(cpCollision.rect.size.width,
                                     cpCollision.rect.size.height);
    }
#endif
    
    for(auto eid : ecs.join<cp::Render, cp::Collision, cp::Position, cp::AI>())
    {
        auto& cpRender = ecs::get<cp::Render>(eid);
        
        if (cpRender.manualPosMode) continue;
        
        auto& cpPosition = ecs::get<cp::Position>(eid);
        auto& cpCollision = ecs::get<cp::Collision>(eid);
     
        cpCollision.current = CollisionComponent::CType::NONE;
        
        if (!ecs::has<cp::Velocity>(eid))
            continue;
        
        cocos2d::Rect bounds = SysHelper::getBounds(cpPosition, cpCollision);
        
        //check room limits
        if (bounds.origin.x < 0)
        {
            bounds.origin.x = 0;
            cpPosition.pos.x = - cpCollision.rect.origin.x;
        }
        
        if (bounds.origin.y < 0)
        {
            bounds.origin.y = 0;
            cpPosition.pos.y = -cpCollision.rect.origin.y;
        }
        
        //check room blocks
        lib::v2i gridPos;
        if (true)
        {
            for(auto rc : this->getRectGridCollisions(bounds, cpCollision.category))
            {
                cpCollision.current = CollisionComponent::CType::DECOR;
                cocos2d::Vec2 cv;
                if (rc.size.width > rc.size.height) // ySlide
                {
                    if (rc.getMinY() > bounds.getMinY())
                        cv.y = -rc.size.height - 1;
                    else
                        cv.y = rc.size.height + 1;
                }
                else //xSlide
                {
                    if (rc.getMinX() > bounds.getMinX())
                        cv.x = -rc.size.width - 1;
                    else
                        cv.x = rc.size.width + 1;
                }
            
                cpPosition.pos += cv;
                bounds.origin += cv;
                if (!this->checkRoomCollision(bounds, cpCollision.category))
                    break;
            }
        }
        
        //check room objects
        for(auto oid : ecs.join<cp::Render, cp::Collision, cp::Position>())
        {
            if (oid != eid)
            {
                auto& cpPosition2 = ecs::get<cp::Position>(oid);
                auto& cpCollision2 = ecs::get<cp::Collision>(oid);
                
                cocos2d::Rect bounds2 = SysHelper::getBounds(cpPosition2, cpCollision2);
                if (bounds2.intersectsRect(bounds))
                {
                    if (cpCollision2.category == CollisionCategory::collectible) //collectible
                    {
                        if (ecs::has<cp::Gear>(eid) && ecs::has<cp::Collectible>(oid))
                        {
                            auto collectible = ModelProvider::instance()->collectible.get(ecs::get<cp::Collectible>(oid));
                            auto& cpGear = ecs::get<cp::Gear>(eid);
                            if (SlotData::checkFreeSlot(cpGear, &collectible))
                            {
                                SlotData::addCollectible(cpGear, &collectible);
                                ecs::get<cp::Render>(oid).sprite->removeFromParent();
                                cp::entity::remove(oid, ecs.getID());
                                this->onGearChanged(eid, cpGear);
                            }
                        }
                    }
                    else //obstacle
                    {
                        cpCollision.current = CollisionComponent::CType::OBJECT;
#if kDrawDebug
                        ecs::get<cp::Render>(eid).collision->setColor(cc::Color3B::RED);
#endif
                        
                        auto diff = slide(bounds, bounds2);
                        cpPosition.pos += diff;
                        
                        if (ecs::has<cp::Velocity>(oid))
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
                                 const CollisionComponent &cpCol,
                                 const cocos2d::Rect& target)
{
    cocos2d::Vec2 pRes = cpPos.last;
    auto b1 = lib::Box(cpPos.last.x + cpCol.rect.getMinX(),
                       cpPos.last.y + cpCol.rect.getMinY(),
                       cpCol.rect.size.width, cpCol.rect.size.height,
                       cpPos.pos.x - cpPos.last.x,
                       cpPos.pos.y - cpPos.last.y);
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
            result.y = -intersect.size.height - 1;
        else
            result.y = intersect.size.height + 1;
    }
    else //xSlide
    {
        if (intersect.getMinX() > src.getMinX())
            result.x = -intersect.size.width - 1;
        else
            result.x = intersect.size.width + 1;
    }
    
    return result;
}