#pragma once
#include "DataGrid.h"
#include "GateInfo.h"

class FloorMapping
{
public:
    FloorMapping(unsigned width, unsigned height):
        size{width,height},
        grid(width, height)
    {
        for(unsigned j = 0; j < size.y; j++)
        for(unsigned i = 0; i < size.x; i++)
            grid[{i,j}] = NONE;
    }

    FloorMapping():FloorMapping(200,200) {}
    
    enum BType { NONE, WALL, GATE};
    
    lib::v2u size;
    
    BType &get(unsigned cx, unsigned cy)
    {
        if (cx >= grid.width || cy >= grid.height)
        {
            return dummy;
        }
        return grid.get(cx, cy);
    }
    
    BType &get(lib::v2i p)
    {
        return get(p.x >=0 ? p.x : 0, p.y >= 0 ? p.y : 0);
    }
    
    BType &get(lib::v2u p)
    {
        return get(p.x, p.y);
    }
    
    void fill(cc::Rect r, BType bType = WALL)
    {
        for(int j = 0; j < r.size.height; j++)
            for(int i = 0; i < r.size.width; i++)
            {
                grid.get(r.origin.x + i, r.origin.y + j) = bType;
            }
    }
    
    lib::v2u getSize()
    {
        return {grid.width, grid.height};
    }
    
    std::list<std::pair<unsigned, GateInfo>>    crossingLeft;
    
private:

    BType dummy = WALL;
    lib::DataGrid<BType>                        grid;
    
};