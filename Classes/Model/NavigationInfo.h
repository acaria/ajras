#pragma once

class IMapData;

#include "Defines.h"
#include "DataGrid.h"

class NavigationInfo
{
public:
    void init(IMapData* data);
    
    //deprecated
    std::list<cc::Vec2> getGridWaypoints(const cc::Vec2& origin,
                                     const cc::Vec2& dest,
                                     def::collision::Cat category);
    
    std::list<cc::Vec2> getWaypoints(unsigned eid,
                                     const cc::Rect& box,
                                     const cc::Vec2& dest,
                                     def::collision::Cat category);
    
    void debugWaypoints(unsigned eid,
                        const cc::Rect& box,
                        const cc::Vec2& dest,
                        cc::DrawNode* drawNode,
                        def::collision::Cat category);
    
private:
    IMapData* data;
};