#include "NavigationInfo.h"
#include "IMapData.h"
#include "PathFinding.h"
#include "NavigationGrid.h"
#include "NavigationGraph.h"
#include "NavigationGraphDebug.h"
#include "CoreLib.h"

std::list<cc::Vec2> NavigationInfo::getGridWaypoints(const cc::Vec2& origin,
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
    
    auto graph = NavigationGrid(data->getCol()->grids[category], agents, data->getTileSize());
    
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

std::list<cc::Vec2> NavigationInfo::getWaypoints(unsigned eid,
                                                 const cc::Rect& box,
                                                 const cc::Vec2& dest,
                                                 def::collision::Cat category)
{
    auto tileSize = data->getTileSize();
    auto agents = this->data->getCol()->getAgentBounds(eid, category);
    
    auto graph = NavigationGraph(data->getCol()->grids[category], box, agents, tileSize);
    
    auto result = lib::PathFinding::aStarSearch(graph,
            graph.getNode({box.getMidX(), box.getMidY()}),
            graph.getNode(dest));

    if (result == nullptr)
        return std::list<cc::Point>();
    
    return result.Value;
}

void NavigationInfo::debugWaypoints(unsigned eid,
                                    const cc::Rect& box,
                                    const cc::Vec2& dest,
                                    cc::DrawNode* drawNode,
                                    def::collision::Cat category)
{
    auto tileSize = data->getTileSize();
    auto agents = this->data->getCol()->getAgentBounds(eid, category);
    
    auto graph = NavigationGraphDebug(data->getCol()->grids[category],
            box, agents, tileSize, drawNode);
    
    auto waypoints = lib::PathFinding::aStarSearch(graph,
        graph.getNode({box.getMidX(), box.getMidY()}),
        graph.getNode(dest));
    
    if (waypoints != nullptr)
    {
        for(auto waypoint : waypoints.Value)
        {
            drawNode->drawSolidCircle(waypoint, 3, 0, 10, cc::Color4F::YELLOW);
        }
    }
}

void NavigationInfo::init(IMapData* data)
{
    this->data = data;
}