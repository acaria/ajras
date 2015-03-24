#include "Headers.h"

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
            auto gridRect = cocos2d::Rect(
                x * blockSize.x, y * blockSize.y, blockSize.x, blockSize.y);
            cocos2d::Vec2 origin = {
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

void CollisionSystem::init(RoomData* room)
{
    this->reset();

    auto grid = room->getModel()->grid;
    
    this->blockSize = room->getModel()->tileSize;
    this->grids[CollisionCategory::walkable] = new lib::DataGrid<bool>(grid.width, grid.height);
    this->grids[CollisionCategory::flyable] = new lib::DataGrid<bool>(grid.width, grid.height);

    for(unsigned j = 0; j < grid.height; j++)
    for(unsigned i = 0; i < grid.width; i++)
    {
        auto fields = grid[{i,j}].fields;
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
    for(auto eid : ecs.join<cp::Render, cp::Collision, cp::Position>())
    {
        auto& cpPosition = ecs::get<cp::Position>(eid);
        auto& cpCollision = ecs::get<cp::Collision>(eid);
     
        cpCollision.collide = false;
        
#if kDrawDebug
        auto& cpRender = ecs::get<cp::Render>(eid);
        cpRender.collision->setVisible(true);
        cpRender.collision->setColor(Color3B::GREEN);
        cpRender.collision->setPosition(cpCollision.rect.origin.x,
                                        cpCollision.rect.origin.y);
        cpRender.collision->setScale(cpCollision.rect.size.width,
                                     cpCollision.rect.size.height);
#endif
        
        if (!ecs::has<cp::Velocity>(eid))
            continue;
        
        auto &cpVel = ecs::get<cp::Velocity>(eid);
        //if (cpVelocity.velocity.isZero())
        //    continue;
        
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
                cpCollision.collide = true;
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
        
        if (eid == 2)
        {
            Log("after: p=%f, c=%d", cpPosition.pos.x, cpCollision.collide?1:0);
        }
        
        /*
        //check room objects
        for(auto oid : ecs.join<cp::Render, cp::Collision, cp::Position>())
        {
            if (oid != eid)
            {
                auto cpPosition2 = ecs::get<cp::Position>(oid);
                auto cpCollision2 = ecs::get<cp::Collision>(oid);
                
                cocos2d::Rect bounds2 = SysHelper::getBounds(cpPosition2, cpCollision2);
                
                if (bounds2.intersectsRect(bounds))
                {
                    cpCollision.collide = true;
#if kDrawDebug
                    ecs::get<cp::Render>(eid).collision->setColor(Color3B::RED);
#endif

                    
                    //bounce
                    auto box = slide(cpPosition, cpCollision, bounds2);
                    cpPosition.pos = {box.x, box.y};
                    cpVelocity.applyVelocity({box.vx, box.vy});
                    
                    if (ecs::has<cp::Input>(eid))
                    {
                        //ecs::get<cp::Render>(eid).cancelAnimation();
                        ecs::get<cp::Input>(eid).predicates.push_back([](unsigned id) {
                            if (!ecs::has<cp::Velocity>(id))
                                return true;
                            return ecs::get<cp::Velocity>(id).velocity.isZero();
                        });
                    }
                }
            }
        }*/
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
    auto translation = Vec2(0,0);
    if (ct < 1.0f)
    {
        translation = {b1.vx * ct, b1.vy * ct};
        auto rt = 1.f - ct;
        b1.vx *= rt;
        b1.vy *= rt;
        if (abs(nX) > 0.0001f) b1.vx = -b1.vx;
        if (abs(nY) > 0.0001f) b1.vy = -b1.vy;
        translation += {b1.vx * rt, b1.vy * rt};
    }
    Log("T=%f,%f", translation.x, translation.y);
    pRes += translation;
    return lib::Box(pRes.x, pRes.y, b1.w, b1.h, b1.vx, b1.vy);
}

lib::Box CollisionSystem::slide(PositionComponent &cpPos,
                                 const CollisionComponent &cpCol,
                                 const cocos2d::Rect& target)
{
    cocos2d::Vec2 pRes = cpPos.last;
    auto b1 = lib::Box(cpPos.last.x + cpCol.rect.getMinX() - 1,
                       cpPos.last.y + cpCol.rect.getMinY() - 1,
                       cpCol.rect.size.width, cpCol.rect.size.height,
                       cpPos.pos.x - cpPos.last.x,
                       cpPos.pos.y - cpPos.last.y);
    auto b2 = lib::Box(target, {0, 0});
    
    float nX, nY;
    auto ct = MAX(0, lib::SweptAABB(b1, b2, nX, nY) - 0.0001f);
    auto translation = Vec2(0,0);
    if (ct < 1.0f)
    {
        translation = {b1.vx * ct, b1.vy * ct};
        auto rt = 1.f - ct;
        auto dotProd = (b1.vx * nY + b1.vy * nX) * rt;
        b1.vx = dotProd * nY * rt;
        b1.vy = dotProd * nX * rt;
        translation += {b1.vx * rt, b1.vy * rt};
    }
    Log("T=%f,%f", translation.x, translation.y);
    pRes += translation;
    return lib::Box(pRes.x, pRes.y, b1.w, b1.h, b1.vx, b1.vy);
}