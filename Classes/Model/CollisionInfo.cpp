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

std::vector<cc::Rect> CollisionInfo::getAvailableBlocks(const cc::Point& pos, unsigned int maxDist, CollisionCategory cat)
{
    return this->getAvailableBlocks(this->data->getCoordFromPos(pos), maxDist, cat);
}

std::vector<cc::Rect> CollisionInfo::getAvailableBlocks(const lib::v2u &coord, unsigned int maxDist, CollisionCategory cat)
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

std::list<cocos2d::Rect> CollisionInfo::getRectGridCollisions(const cocos2d::Rect& rect, CollisionCategory cat)
{
    auto moveAble = this->grids[cat];
    auto res = std::list<cocos2d::Rect>();
    
    auto upLeft = this->data->getCoordFromPos({rect.getMinX() - 1, rect.getMaxY() +  1});
    auto downRight = this->data->getCoordFromPos({rect.getMaxX() + 1, rect.getMinY() - 1});
    
    for(unsigned x = upLeft.x; x <= downRight.x; x++)
        for(unsigned y = downRight.y; y <= upLeft.y; y++)
        {
            if (!moveAble->get({x,y}))
            {
                auto gridRect = this->data->getBlockBound({x, y});
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