#include "NavigationGraph.h"

NavigationGraph::NavigationGraph(lib::DataGrid<bool>& content):content(content)
{
}

bool NavigationGraph::getState(const lib::v2i &coord)
{
    if (coord.x < 0 || coord.x >= content.width)
        return false;
    if (coord.y < 0 || coord.y >= content.height)
        return false;
    return content.get(coord.x,coord.y);
}

std::vector<NavigationGraph::Node> NavigationGraph::neighbors(Node node)
{
    std::vector<Node> result;
    
    for(int x = -1; x <= 1; x++)
        for(int y = -1; y <= 1; y++)
        {
            if (abs(x) == abs(y))
                continue;
            if (content.get(node.x + x, node.y + y))
                result.push_back({(unsigned)(node.x + x),(unsigned)(node.y + y)});
        }
    
    if (result.size() == 4)
    {
        for(int x = -1; x <= 1; x++)
            for(int y = -1; y <= 1; y++)
            {
                if (abs(x) != abs(y) || (x == 0 && y == 0))
                    continue;
                if (content.get(node.x + x, node.y + y))
                    result.push_back({(unsigned)(node.x + x),(unsigned)(node.y + y)});
            }
    }
    
    return result;
}

int NavigationGraph::cost(Node n1, Node n2)
{
    return 1;
}

int NavigationGraph::heuristic(Node n1, Node n2)
{
    return abs((int)n1.x - (int)n2.x) + abs((int)n1.y - (int)n2.y);
}
