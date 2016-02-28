#pragma once

#include "DataGrid.h"
#include "Defines.h"

//Navigation structure based on grid coords + agents coords
struct NavigationGrid
{
    using Agent = def::collision::Agent;
    typedef lib::v2u Node;
    
    NavigationGrid(lib::DataGrid<bool>& content, std::list<Agent>& agents,
                   const cc::Size& tileSize);
    
    std::list<Node> neighbors(Node node);
    int cost(Node n1, Node n2);
    int heuristic(Node n1, Node n2);

private:
    bool check(int x, int y);
    lib::DataGrid<bool>&    content;
    std::list<Agent>&       agents;
    cc::Size                tileSize;
};