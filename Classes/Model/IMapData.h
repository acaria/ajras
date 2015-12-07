#pragma once

#include "DataGrid.h"
#include "BlockInfo.h"
#include "CollisionInfo.h"
#include "NavigationInfo.h"

struct IMapData
{
    virtual ~IMapData() {}
    virtual lib::DataGrid<BlockInfo>& getGrid() = 0;
    virtual cc::Size getTileSize() = 0;
    virtual cc::Rect getBlockBound(const lib::v2u& coord) = 0;
    virtual lib::v2u getCoordFromPos(const cocos2d::Vec2& pos) = 0;
    virtual cc::Rect getBounds() = 0;
    virtual CollisionInfo* getCol() = 0;
    virtual NavigationInfo* getNav() = 0;
};