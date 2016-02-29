#pragma once

#pragma once

#include "NavigationGraph.h"

//Navigation structure based on grid coords + agents coords
struct NavigationGraphDebug : public NavigationGraph
{
    NavigationGraphDebug(const lib::DataGrid<bool>& content,
                         const cc::Rect& actor,
                         const std::list<cc::Rect>& agents,
                         const cc::Size& tileSize,
                         cc::DrawNode* drawNode);
    
    virtual std::list<Node> neighbors(Node node) override;
    
private:
    cc::DrawNode*           drawNode;
};