#pragma once

#pragma once

#include "DataGrid.h"
#include "Defines.h"
#include <array>

//Navigation structure based on grid coords + agents coords
struct NavigationGraph
{
    using Agent = def::collision::Agent;
    typedef cc::Point Node;
    
    NavigationGraph(const lib::DataGrid<bool>& content,
                    const cc::Rect& actor,
                    const std::list<Agent>& agents,
                    const cc::Size& tileSize);
    
    virtual std::list<Node> neighbors(Node node);
    int cost(Node n1, Node n2);
    int heuristic(Node n1, Node n2);
    
    Node getNode(const cc::Point& pt);
    
protected:
    
    //input
    lib::DataGrid<bool>     content;
    const std::list<Agent>& agents;
    std::map<Node, int>     costMap;
    cc::Rect                actor;
    cc::Size                tileSize;
};