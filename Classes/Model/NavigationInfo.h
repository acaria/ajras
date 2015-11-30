#pragma once

class IMapData;

#include "Defines.h"
#include "DataGrid.h"

class NavigationInfo
{
public:
    void init(IMapData* data);
    
    
    std::list<cc::Vec2> getWaypoints(const cc::Vec2& origin,
                                     const cc::Vec2& dest,
                                     def::collision::Cat category);
    
    
private:
    IMapData* data;
};