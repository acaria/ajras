#pragma once

#include "DataGrid.h"
#include "Defines.h"

struct NavigationGraph
{
    using Agent = def::collision::Agent;
    typedef lib::v2u Node;
    
    NavigationGraph(lib::DataGrid<bool>& content, std::list<Agent>& agents, const cc::Size& tileSize);
    
    std::vector<Node> neighbors(Node node);
    int cost(Node n1, Node n2);
    int heuristic(Node n1, Node n2);

private:
    bool check(int x, int y);
    lib::DataGrid<bool>&    content;
    std::list<Agent>&       agents;
    cc::Size                tileSize;
};