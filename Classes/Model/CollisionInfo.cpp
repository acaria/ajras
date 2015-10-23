#include "CollisionInfo.h"

CollisionInfo::~CollisionInfo()
{
    this->reset();
}

void CollisionInfo::reset()
{
    for(auto pair : this->grids)
    {
        if (pair.second != nullptr)
            delete pair.second;
    }
    this->grids.clear();
}

void CollisionInfo::init(IMapData *map)
{
    this->data = map;
}

void CollisionInfo::process()
{
    this->reset();
    using cat = CollisionCategory;
    
    lib::v2u dim = {data->getGrid().width, data->getGrid().height};
    
    this->grids[cat::walkable] = new lib::DataGrid<bool>(dim);
    this->grids[cat::flyable] = new lib::DataGrid<bool>(dim);
    
    for(unsigned j = 0; j < dim.y; j++)
        for(unsigned i = 0; i < dim.x; i++)
        {
            auto fields = data->getGrid().get(i,j).fields;
            if (fields.find(BlockInfo::PType::collision) != fields.end())
            {
                auto category = fields[BlockInfo::PType::collision];
                if (category == "walkable")
                {
                    this->grids[cat::flyable]->get({i, j}) = true;
                    this->grids[cat::walkable]->get({i, j}) = true;
                }
                else if (category == "flyable")
                    this->grids[cat::flyable]->get({i, j}) = true;
                else
                    Log("invalid collision category: %s", category.c_str());
            }
        }
}

std::vector<cc::Rect> CollisionInfo::getNearEmptyBlocks(const cc::Point& pos, unsigned int maxDist, CollisionCategory cat)
{
    return this->getNearEmptyBlocks(this->data->getCoordFromPos(pos), maxDist, cat);
}

std::vector<cc::Rect> CollisionInfo::getNearEmptyBlocks(const lib::v2u &coord, unsigned int maxDist, CollisionCategory cat)
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
        if (this->grids[cat]->get({(unsigned)i, (unsigned)j}))
            results.push_back(data->getBlockBound({(unsigned)i,(unsigned)j}));
    }
    
    if (results.size() == 0)
        return {this->data->getBlockBound(coord)};
    return results;
}

bool CollisionInfo::checkRoomCollision(const cocos2d::Rect &rect, CollisionCategory cat)
{
    auto moveAble = this->grids[cat];
    if (!moveAble->get(this->data->getCoordFromPos({rect.getMinX(), rect.getMinY()})))
        return true;
    if (!moveAble->get(this->data->getCoordFromPos({rect.getMinX(), rect.getMaxY()})))
        return true;
    if (!moveAble->get(this->data->getCoordFromPos({rect.getMaxX(), rect.getMinY()})))
        return true;
    if (!moveAble->get(this->data->getCoordFromPos({rect.getMaxX(), rect.getMaxY()})))
        return true;
    return false;
}

bool CollisionInfo::needMerge(const cc::Rect &r1, const cc::Rect &r2)
{
    if (r1.size.width != r2.size.width && r1.size.height != r2.size.height)
        return false;
    if (r1.size.width == r2.size.width && r1.origin.x == r2.origin.x)
    {
        if (r1.getMinY() == r2.getMaxY() || r2.getMinY() == r1.getMaxY())
            return true;
    }
    if (r1.size.height == r2.size.height && r1.origin.y == r2.origin.y)
    {
        if (r1.getMinX() == r2.getMaxX() || r2.getMinX() == r1.getMaxX())
            return true;
    }
    
    return false;
}

std::list<cc::Rect> CollisionInfo::mergeRectGrids(std::list<cc::Rect> src)
{
    struct processInfo
    {
        cc::Rect bounds;
        bool processed;
    };
    
    std::list<processInfo> processList;
    for(auto bounds : src)
    {
        processList.push_back({
            .bounds = bounds,
            .processed = false
        });
    }
    
    std::list<cc::Rect> res;
    
    for(auto& info : processList)
    {
        if (info.processed)
            continue;
        cc::Rect mRect = info.bounds;
        for(auto& info2 : processList)
        {
            auto bounds = info2.bounds;
            if (needMerge(bounds, mRect))
            {
                info2.processed = true;
                cc::Point origin = {
                    MIN(bounds.origin.x, mRect.origin.x),
                    MIN(bounds.origin.y, mRect.origin.y)
                };
                mRect = {
                    origin.x, origin.y,
                    MAX(bounds.getMaxX(), mRect.getMaxX()) - origin.x,
                    MAX(bounds.getMaxY(), mRect.getMaxY()) - origin.y
                };
            }
        }
        res.push_back(mRect);
        info.processed = true;
    }
    
    return res;
}

std::list<cocos2d::Rect> CollisionInfo::getRectGridCollisions(const cocos2d::Rect& rect, CollisionCategory cat)
{
    auto moveAble = this->grids[cat];
    auto res = std::list<cocos2d::Rect>();
    
    auto upLeft = this->data->getCoordFromPos({rect.getMinX(), rect.getMaxY()});
    auto downRight = this->data->getCoordFromPos({rect.getMaxX(), rect.getMinY()});
    
    for(unsigned x = upLeft.x; x <= downRight.x; x++)
    for(unsigned y = downRight.y; y <= upLeft.y; y++)
    {
        if (!moveAble->get({x,y}))
        {
            res.push_back(lib::getIntersection(
                    this->data->getBlockBound({x, y}), rect));
        }
    }
    
    auto mergedRes = this->mergeRectGrids(res);
    
    mergedRes.sort([&rect](const cocos2d::Rect& r1, const cocos2d::Rect& r2){
        return r1.size.width * r1.size.height > r2.size.width * r2.size.height;
    });
    
    return mergedRes;
}