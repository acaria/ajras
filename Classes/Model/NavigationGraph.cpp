#include "NavigationGraph.h"
#include "CoreLib.h"

NavigationGraph::NavigationGraph(const lib::DataGrid<bool>& contentRef,
                                 const cc::Rect& actor,
                                 const std::list<Agent>& agents,
                                 const cc::Size& tileSize):
content(contentRef), actor(actor), agents(agents), tileSize(tileSize)
{
    for(auto agent : agents)
    {
        auto b = agent.bounds;
        unsigned maxX = b.getMaxX() / tileSize.width;
        unsigned maxY = b.getMaxY() / tileSize.height;
        for(unsigned x = b.getMinX() / tileSize.width; x <= maxX; x++)
            for(unsigned y = b.getMinY() / tileSize.height; y <= maxY; y++)
            {
                cc::Point point = {x * tileSize.width + tileSize.width / 2,
                    y * tileSize.height + tileSize.height / 2};
                if (costMap.find(point) == costMap.end())
                    costMap[point] = 1;
                else
                    costMap[point]++;
            }
    }
}

std::list<NavigationGraph::Node> NavigationGraph::neighbors(Node node)
{
    std::list<Node> neighbors;
    
    lib::v2u coord = {unsigned(node.x / tileSize.width),
        unsigned(node.y / tileSize.width)};
    
    for(int x = -1; x <= 1; x++)
        for(int y = -1; y <= 1; y++)
        {
            if (abs(x) == abs(y))
                continue;
            if (content.get(coord.x + x,coord.y + y))
                neighbors.push_back({node.x + (x * tileSize.width),node.y + (y * tileSize.height)});
        }
    
    if (neighbors.size() == 4)
    {
        for(int x = -1; x <= 1; x++)
            for(int y = -1; y <= 1; y++)
            {
                if (abs(x) != abs(y) || (x == 0 && y == 0))
                    continue;
                if (content.get(coord.x + x,coord.y + y))
                    neighbors.push_back({node.x + (x * tileSize.width),node.y + (y * tileSize.height)});
            }
    }
    
    auto nodeBounds = cc::Rect {
        node.x - actor.size.width / 2, node.y - actor.size.height / 2,
        actor.size.width, actor.size.height
    };
    
    return neighbors;
}

NavigationGraph::Node NavigationGraph::getNode(const cc::Point& pt)
{
    return {
        unsigned(pt.x / tileSize.width) * tileSize.width + tileSize.width / 2,
        unsigned(pt.y / tileSize.height) * tileSize.height + tileSize.height / 2
    };
}

int NavigationGraph::cost(Node n1, Node n2)
{
    int costValue = 1;
    
    if (costMap.find(n1) != costMap.end())
        costValue += costMap[n1] * 100;
    if (costMap.find(n2) != costMap.end())
        costValue += costMap[n2] * 100;
    
    return costValue;
}

int NavigationGraph::heuristic(Node n1, Node n2)
{
    return abs((int)n1.x - (int)n2.x) + abs((int)n1.y - (int)n2.y);
}