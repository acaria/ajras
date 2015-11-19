#include "NavigationInfo.h"
#include "IMapData.h"
#include "PathFinding.h"

std::list<cc::Vec2> NavigationInfo::getWaypoints(const cc::Vec2& origin, const cc::Vec2& dest, CollisionCategory category)
{
    std::list<cc::Vec2> result;
    
    auto graph = GridGraph(*data->getCol()->grids[category]);
    auto path = lib::PathFinding::breadthFirstSearch(graph,
            data->getCoordFromPos(origin), data->getCoordFromPos(dest));

    if (path != nullptr && path->size() > 0)
    {
        for(auto coord : path.Value)
        {
            auto block = data->getBlockBound(coord);
            result.push_back({block.getMidX(), block.getMidY()});
        }
    }
    
    return result;
}

void NavigationInfo::init(IMapData* data)
{
    this->data = data;
}