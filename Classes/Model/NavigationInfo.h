#pragma once

class IMapData;

#include "CoreLib.h"
#include "Defines.h"
#include "DataGrid.h"

class NavigationInfo
{
public:
    
    struct GridGraph
    {
        typedef lib::v2u Node;
    
        GridGraph(lib::DataGrid<bool>& content):content(content){}
        
        bool getState(const lib::v2i &coord)
        {
            if (coord.x < 0 || coord.x >= content.width)
                return false;
            if (coord.y < 0 || coord.y >= content.height)
                return false;
            return content.get(coord.x,coord.y);
        }
        
        std::vector<Node> neighbors(Node node)
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
        
        int cost(Node n1, Node n2)
        {
            return 1;
        }
        
        int heuristic(Node n1, Node n2)
        {
            return abs((int)n1.x - (int)n2.x) + abs((int)n1.y - (int)n2.y);
        }
        
    private:
        lib::DataGrid<bool>& content;
    };
    
    void init(IMapData* data);
    
    
    std::list<cc::Vec2> getWaypoints(const cc::Vec2& origin,
                                     const cc::Vec2& dest,
                                     CollisionCategory category);
    
    
private:
    IMapData* data;
};