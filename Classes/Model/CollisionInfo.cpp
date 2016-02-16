#include "CollisionInfo.h"
#include "IMapData.h"
#include "CollisionEngine.h"

void CollisionInfo::reset()
{
    this->grids.clear();
}

void CollisionInfo::init(IMapData *map)
{
    this->data = map;
    this->prepare();
}

void CollisionInfo::process()
{
    for(auto pair : this->agents)
    {
        this->processAgent(pair.second);
    }
    
    for(auto pair : this->fakeNodeAgents)
    {
        this->processFakeNode(pair.first, pair.second);
    }
}

void CollisionInfo::processAgent(const Agent &agent)
{
    if (agent.bounds.origin == agent.lastBounds.origin)
        return;
    
    for (auto pair2 : this->agents)
    {
        auto& other = pair2.second;
        
        if (other.id == agent.id) continue;
        
        if (agent.bounds.intersectsRect(other.bounds))
        {
            auto diff = CollisionEngine::slide(agent.bounds, other.bounds);
            if (!diff.isZero())
                this->onAgentCollision(agent.id, other.id, diff * (1.001));
        }
    }
    
    if (checkCollisionRect(lib::getUnion(agent.bounds, agent.lastBounds), agent.category))
    {
        auto diff = this->getCollisionDiff(agent.bounds, agent.lastBounds, agent.category);
        if (!diff.isZero())
            this->onDecorCollision(agent.id, diff * (1.001));
    }
}

void CollisionInfo::processFakeNode(cc::Node* node, const cc::Rect& shape)
{
    auto bounds = cc::Rect(node->getPosition() + shape.origin, shape.size);
    
    for (auto pair : this->agents)
    {
        auto& other = pair.second;
        
        if (bounds.intersectsRect(other.bounds))
        {
            auto diff = CollisionEngine::slide(bounds, other.bounds);
            if (!diff.isZero())
                this->onFakeNodeCollision(other.id, diff * (1.001));
        }
    }
}

void CollisionInfo::prepare()
{
    this->reset();
    using cat = def::collision::Cat;
    
    lib::v2u dim = {data->getGrid().width, data->getGrid().height};
    
    this->grids[cat::walkable] = lib::DataGrid<bool>(dim);
    this->freeStaticCoords[cat::walkable] = std::vector<lib::v2u>();
    
    this->grids[cat::flyable] = lib::DataGrid<bool>(dim);
    this->freeStaticCoords[cat::flyable] = std::vector<lib::v2u>();
    
    for(unsigned j = 0; j < dim.y; j++)
    for(unsigned i = 0; i < dim.x; i++)
    {
        auto fields = data->getGrid().get(i,j).fields;
        if (fields.find(BlockInfo::PType::collision) != fields.end())
        {
            auto category = fields[BlockInfo::PType::collision];
            if (category == "walkable")
            {
                this->grids[cat::flyable][{i, j}] = true;
                this->grids[cat::walkable][{i, j}] = true;
                
                if (!lib::hasKey(data->getGrid()[{i,j}].fields, BlockInfo::gating))
                {
                    this->freeStaticCoords[cat::flyable].push_back({i,j});
                    this->freeStaticCoords[cat::walkable].push_back({i,j});
                }
            }
            else if (category == "flyable")
            {
                this->grids[cat::flyable][{i, j}] = true;
                if (!lib::hasKey(data->getGrid()[{i,j}].fields, BlockInfo::gating))
                {
                    this->freeStaticCoords[cat::flyable].push_back({i,j});
                }
            }
            else
                Log("invalid collision category: %s", category.c_str());
        }
    }
}

std::vector<lib::v2u> CollisionInfo::getAllStaticFreeCoords(ColCat cat)
{
    return this->freeStaticCoords[cat];
}

std::vector<cc::Rect> CollisionInfo::getNearEmptyBlocks(const cc::Point& pos, unsigned int maxDist, def::collision::Cat cat)
{
    return this->getNearEmptyBlocks(this->data->getCoordFromPos(pos), maxDist, cat);
}

std::vector<cc::Rect> CollisionInfo::getNearEmptyBlocks(const lib::v2u &coord, unsigned int maxDist, def::collision::Cat cat)
{
    std::vector<cc::Rect> results;
    for(int j = coord.y - maxDist; j <= coord.y + maxDist; j++)
    for(int i = coord.x - maxDist; i <= coord.x + maxDist; i++)
    {
        if (i < 0 || i >= this->data->getGrid().width)
            continue;
        if (j < 0 || j >= this->data->getGrid().height)
            continue;
        
        if (i == coord.x && j == coord.y)
            continue;
        if (this->grids[cat][{(unsigned)i, (unsigned)j}])
            results.push_back(data->getBlockBound({(unsigned)i,(unsigned)j}));
    }
    
    if (results.size() == 0)
        return {this->data->getBlockBound(coord)};
    return results;
}


cc::Point CollisionInfo::getCollisionDiff(const cc::Rect& destBounds, const cc::Rect& lastBounds, def::collision::Cat cat)
{
    cc::Point dir = destBounds.origin - lastBounds.origin;
    
    auto sweep = CollisionEngine::interpolateDir(dir, lastBounds, data->getTileSize());
    
    int cp = 0;
    for(cc::Point pos = lastBounds.origin + sweep.tx; cp <= sweep.count; cp++, pos += sweep.dir)
    {
        cc::Rect bounds = {pos, lastBounds.size};
        auto collisions = this->getRectGridCollisions(bounds, cat);
        if (collisions.size() > 0)
        {
            for(auto rc : collisions)
            {
                cocos2d::Vec2 cv;
        
                if (rc.size.width > rc.size.height) //ySlide
                    cv.y = rc.size.height * (rc.getMinY() > bounds.getMinY() ? -1 : 1);
                else //xSlide
                    cv.x = rc.size.width * (rc.getMinX() > bounds.getMinX() ? -1 : 1);
        
                bounds.origin += cv;
        
                if (!this->checkCollisionRect(bounds, cat))
                    break;
            }
            return bounds.origin - destBounds.origin;
        }
    }
    return {0,0};
}

bool CollisionInfo::checkCollisionRect(const cc::Rect &rect, def::collision::Cat cat)
{
    if (rect.size.width == 0 || rect.size.height == 0)
        return false;
    auto moveAble = this->grids[cat];
 
    auto downLeft = this->data->getCoordFromPos({rect.getMinX(), rect.getMinY()});
    auto upRight = this->data->getCoordFromPos({rect.getMaxX(), rect.getMaxY()});
    
    for(unsigned x = downLeft.x; x <= upRight.x; x++)
    for(unsigned y = downLeft.y; y <= upRight.y; y++)
    {
        if (!moveAble[{x, y}])
            return true;
    }

    return false;
}

bool CollisionInfo::checkCollisionRay(const cc::Point& origin,
                                      const cc::Point& dest,
                                      def::collision::Cat cat)
{
    auto dir = dest - origin;
    if (dir.x == 0 && dir.y == 0)
        return false;
    auto& moveAble = this->grids[cat];
    cc::Point p1 = origin;
    cc::Point p2 = origin + dir;
    auto downLeft = this->data->getCoordFromPos({MIN(p1.x, p2.x), MIN(p1.y, p2.y)});
    auto upRight = this->data->getCoordFromPos({MAX(p1.x, p2.x), MAX(p1.y, p2.y)});
    
    auto nDir = dir.getNormalized();
    cc::Point dirFrac = {
        1.0f / (nDir.x == 0 ? FLT_MIN : nDir.x),
        1.0f / (nDir.y == 0 ? FLT_MIN : nDir.y)
    };
    
    for(unsigned x = downLeft.x; x <= upRight.x; x++)
    for(unsigned y = downLeft.y; y <= upRight.y; y++)
    {
        if (!moveAble[{x, y}])
        {
            cc::Rect bounds = this->data->getBlockBound({x,y});
            float t1 = (bounds.getMinX() - origin.x) * dirFrac.x;
            float t2 = (bounds.getMaxX() - origin.x) * dirFrac.x;
            float t3 = (bounds.getMinY() - origin.y) * dirFrac.y;
            float t4 = (bounds.getMaxY() - origin.y) * dirFrac.y;
            
            float tmin = MAX(MIN(t1, t2), MIN(t3, t4));
            float tmax = MIN(MAX(t1, t2), MAX(t3, t4));
            
            if (tmax >= 0 && tmin <= tmax)
                return true;
        }
    }
    
    return false;
}

std::list<cocos2d::Rect> CollisionInfo::getRectGridCollisions(const cocos2d::Rect& rect, def::collision::Cat cat)
{
    auto moveAble = this->grids[cat];
    auto res = std::list<cocos2d::Rect>();
    
    auto upLeft = this->data->getCoordFromPos({rect.getMinX(), rect.getMaxY()});
    auto downRight = this->data->getCoordFromPos({rect.getMaxX(), rect.getMinY()});
    
    for(unsigned x = upLeft.x; x <= downRight.x; x++)
        for(unsigned y = downRight.y; y <= upLeft.y; y++)
        {
            if (!moveAble[{x,y}])
            {
                res.push_back(lib::getIntersection(
                                                   this->data->getBlockBound({x, y}), rect));
            }
        }
    
    auto mergedRes = CollisionEngine::mergeRectGrids(res);
    
    mergedRes.sort([&rect](const cocos2d::Rect& r1, const cocos2d::Rect& r2){
        return r1.size.width * r1.size.height > r2.size.width * r2.size.height;
    });
    
    return mergedRes;
}
