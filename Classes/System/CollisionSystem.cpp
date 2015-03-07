#include "Headers.h"

CollisionSystem::~CollisionSystem()
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
    if (!moveAble->get(this->getGridPosIntersect({rect.getMinX(), rect.getMaxY() - 1})))
        return true;
    if (!moveAble->get(this->getGridPosIntersect({rect.getMaxX() - 1, rect.getMinY()})))
        return true;
    if (!moveAble->get(this->getGridPosIntersect({rect.getMaxX() - 1, rect.getMaxY() - 1})))
        return true;
    return false;
}

std::list<cocos2d::Rect> CollisionSystem::getRectGridCollisions(const cocos2d::Rect& rect, CollisionCategory cat)
{
    auto moveAble = this->grids[cat];
    auto res = std::list<cocos2d::Rect>();
    
    auto upLeft = this->getGridPosIntersect(rect.getMinX(), rect.getMaxY());
    auto downRight = this->getGridPosIntersect(rect.getMaxX(), rect.getMinY());
    
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
    auto grid = room->getModel()->grid;
    
    for(auto pair : this->grids)
    {
        if (pair.second != nullptr)
            delete pair.second;
    }
    this->grids.clear();
    
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
        auto& cpVelocity = ecs::get<cp::Velocity>(eid);
        
        if (cpVelocity.velocity.isZero())
            continue;
        
        cocos2d::Rect bounds = {
            cpPosition.pos.x + cpCollision.rect.origin.x,
            cpPosition.pos.y + cpCollision.rect.origin.y,
            cpCollision.rect.size.width,
            cpCollision.rect.size.height
        };
        
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
        if (ecs::has<cp::Input>(eid))
        {
            for(auto rc : this->getRectGridCollisions(bounds, cpCollision.category))
            {
                cocos2d::Vec2 cv;
                if (rc.size.width > rc.size.height) // ySlide
                {
                    if (rc.getMinY() > bounds.getMinY())
                        cv.y = -rc.size.height;
                    else
                        cv.y = rc.size.height;
                }
                else //xSlide
                {
                    if (rc.getMinX() > bounds.getMinX())
                        cv.x = -rc.size.width;
                    else
                        cv.x = rc.size.width;
                }
            
                cpPosition.pos += cv;
                bounds.origin += cv;
                if (!this->checkRoomCollision(bounds, cpCollision.category))
                    break;
            }
        }
    }
}

void CollisionSystem::animate(double dt, double tp)
{
    
}