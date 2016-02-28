#include "NavigationGrid.h"
#include "CoreLib.h"

NavigationGrid::NavigationGrid(lib::DataGrid<bool>& content,
                                 std::list<Agent>& agents,
                                 const cc::Size& tileSize):
    content(content), agents(agents), tileSize(tileSize)
{
}

bool NavigationGrid::check(int x, int y)
{
    for(auto agent : agents)
    {
        if (agent.bounds.containsPoint({x * tileSize.width + tileSize.width / 2,
                                        y * tileSize.height + tileSize.height / 2}))
            return false;
    }
    return content.get(x,y);
}

std::list<NavigationGrid::Node> NavigationGrid::neighbors(Node node)
{
    std::list<Node> result;
    
    for(int x = -1; x <= 1; x++)
    for(int y = -1; y <= 1; y++)
    {
        if (abs(x) == abs(y))
            continue;
        if (check(node.x + x, node.y + y))
            result.push_back({(unsigned)(node.x + x),(unsigned)(node.y + y)});
    }
    
    if (result.size() == 4)
    {
        for(int x = -1; x <= 1; x++)
            for(int y = -1; y <= 1; y++)
            {
                if (abs(x) != abs(y) || (x == 0 && y == 0))
                    continue;
                if (check(node.x + x, node.y + y))
                    result.push_back({(unsigned)(node.x + x),(unsigned)(node.y + y)});
            }
    }
    
    return result;
}

int NavigationGrid::cost(Node n1, Node n2)
{
    return 1;
}

int NavigationGrid::heuristic(Node n1, Node n2)
{
    return abs((int)n1.x - (int)n2.x) + abs((int)n1.y - (int)n2.y);
}
