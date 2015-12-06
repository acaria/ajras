#include "NavigationInfo.h"
#include "IMapData.h"
#include "PathFinding.h"
#include "NavigationGraph.h"
#include "CoreLib.h"

std::list<cc::Vec2> NavigationInfo::getWaypoints(const cc::Vec2& origin,
                                                 const cc::Vec2& dest,
                                                 def::collision::Cat category)
{
    std::list<cc::Vec2> result;
    
    auto agents = linq::from(this->data->getCol()->agents)
        >> linq::select([](std::pair<unsigned, def::collision::Agent> element) {
            return element.second; })
        >> linq::where([category](def::collision::Agent agent) {
            return agent.category == category; })
        >> linq::to_list();
    
    auto graph = NavigationGraph(data->getCol()->grids[category], agents, data->getTileSize());
    
    auto path = lib::PathFinding::aStarSearch(graph,
                                              data->getCoordFromPos(origin),
                                              data->getCoordFromPos(dest));

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