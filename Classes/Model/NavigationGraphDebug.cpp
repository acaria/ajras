#include "NavigationGraphDebug.h"
#include "CoreLib.h"

NavigationGraphDebug::NavigationGraphDebug(const lib::DataGrid<bool>& contentRef,
                                           const cc::Rect& actor,
                                           const std::list<cc::Rect>& agents,
                                           const cc::Size& tileSize,
                                           cc::DrawNode* drawNode):
NavigationGraph(contentRef, actor, agents, tileSize), drawNode(drawNode)
{
}

std::list<NavigationGraphDebug::Node> NavigationGraphDebug::neighbors(Node node)
{
    auto neighbors = NavigationGraph::neighbors(node);
    
    auto nodeBounds = cc::Rect {
        node.x - actor.size.width / 2, node.y - actor.size.height / 2,
        actor.size.width, actor.size.height
    };
    
    drawNode->drawRect(nodeBounds.origin, nodeBounds.origin + nodeBounds.size, cc::Color4F::RED);
    for(auto neighbor : neighbors)
        drawNode->drawLine({node.x, node.y}, {neighbor.x, neighbor.y}, cc::Color4F::YELLOW);
    
    return neighbors;
}