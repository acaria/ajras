#pragma once
#include "Headers.h"

class MapShape
{
public:
    MapShape(unsigned width, unsigned height):
        size{width,height},
        grid(size.x, size.y)
    {
        for(unsigned j = 0; j < size.y; j++)
        for(unsigned i = 0; i < size.x; i++)
            grid[{i,j}] = NONE;
    }

    MapShape():MapShape(120,120) {}
    
    enum BType { NONE, WALL, GATE };
    
    lib::v2u size;
    
    BType &get(int x, int y)
    {
        if (x >= grid.width || y >= grid.height)
        {
            dummy = WALL;
            return dummy;
        }
        return grid.get(x, y);
    }
    
    BType &get(lib::v2i p)
    {
        return get(p.x>=0?p.x:0, p.y>=0?p.y:0);
    }
    
    BType &get(lib::v2u p)
    {
        return get(p.x, p.y);
    }
    
    void fill(cocos2d::Rect r, BType t)
    {
        for(int j = 0; j < r.size.height; j++)
        for(int i = 0; i < r.size.width; i++)
        {
            grid.get(r.origin.x + i, r.origin.y + j) = t;
        }
    }
    
    lib::v2u getSize()
    {
        return {grid.width, grid.height};
    }
    
private:
    BType dummy;
    lib::DataGrid<BType> grid;
};