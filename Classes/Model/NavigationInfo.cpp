#include "NavigationInfo.h"
#include "IMapData.h"

std::list<cc::Vec2> NavigationInfo::getWaypoints(cc::Vec2 origin, cc::Vec2 dest, CollisionCategory category)
{
    std::list<cc::Vec2> result;
    
    auto path = getPathCoords(data->getCoordFromPos(origin), data->getCoordFromPos(dest), category);
    if (path != nullptr && path->size() > 0)
    {
        path->pop_front();
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

std::list<lib::v2u> NavigationInfo::getNeighborCoords(const lib::v2u& coord, CollisionCategory cat)
{
    std::list<lib::v2u> result;
    auto grid = this->data->getCol()->grids[cat];
    
    for(int x = -1; x <= 1; x++)
    for(int y = -1; y <= 1; y++)
    {
        if (x == 0 && y == 0)
            continue;
        if (grid->get(coord.x + x, coord.y + y))
            result.push_back({(unsigned)x,(unsigned)y});
    }
    
    return result;
}

bool NavigationInfo::getCollisionState(const lib::v2i &coord, CollisionCategory cat)
{
    if (coord.x < 0 || coord.x >= data->getGrid().width)
        return false;
    if (coord.y < 0 || coord.y >= data->getGrid().height)
        return false;
    auto grid = this->data->getCol()->grids[cat];
    return grid->get(coord.x, coord.y);
}

lib::Nullable<std::list<lib::v2u>> NavigationInfo::getPathCoords(lib::v2u from, lib::v2u to, CollisionCategory category)
{
    std::queue<lib::v2u> frontier;
    std::map<lib::v2u, lib::v2u> cameFrom;
    frontier.push(from);
    cameFrom[from] = lib::v2u(0,0);
    
    while(frontier.size() > 0)
    {
        auto current = frontier.front();
        frontier.pop();
        for(auto coord : this->getNeighborCoords(current, category))
        {
            frontier.push(coord);
            cameFrom[coord] = current;
        }
    }
    
    if (frontier.size() == 0)
        return nullptr;
    
    std::list<lib::v2u> result;
    
    lib::v2u current = to;
    while(current != from)
    {
        result.push_back(current);
        current = cameFrom[current];
    }
    
    result.reverse();
    
    return result;
}