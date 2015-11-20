#include "NavigationInfo.h"
#include "IMapData.h"
#include "PathFinding.h"
#include "NavigationGraph.h"

std::list<cc::Vec2> NavigationInfo::getWaypoints(const cc::Vec2& origin, const cc::Vec2& dest, def::collision::Cat category)
{
    std::list<cc::Vec2> result;
    
    auto graph = NavigationGraph(*data->getCol()->grids[category]);
    auto path = lib::PathFinding::aStarSearch(graph,
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