#pragma once

class IMapData;

#include "CoreLib.h"
#include "Defines.h"

class NavigationInfo
{
public:
    void init(IMapData* data);
    
    
    std::list<cc::Vec2> getWaypoints(const cc::Vec2& origin,
                                     const cc::Vec2& dest,
                                     CollisionCategory category);
    
    
private:
    lib::Nullable<std::list<lib::v2u>> getPathCoords(lib::v2u from, lib::v2u to,
            CollisionCategory category);
    
    std::list<lib::v2u> getNeighborCoords(const lib::v2u& coord, CollisionCategory cat);
    bool getCollisionState(const lib::v2i& coord, CollisionCategory cat);

    IMapData* data;
};