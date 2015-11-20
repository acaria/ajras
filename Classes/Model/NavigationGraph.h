#pragma once

#include "CoreLib.h"
#include "DataGrid.h"

struct NavigationGraph
{
    typedef lib::v2u Node;
    
    NavigationGraph(lib::DataGrid<bool>& content);
    
    bool getState(const lib::v2i &coord);
    std::vector<Node> neighbors(Node node);
    int cost(Node n1, Node n2);
    int heuristic(Node n1, Node n2);

private:
    lib::DataGrid<bool>& content;
};