#pragma once

#include "DataGrid.h"
#include "BlockInfo.h"
#include "Event.h"

class IMapData
{
public:
    virtual ~IMapData() {}
    virtual lib::DataGrid<BlockInfo>& getGrid() = 0;
    virtual cc::Size getTileSize() = 0;
    virtual cc::Rect getBlockBound(const lib::v2u& coord) = 0;
    virtual lib::v2u getCoordFromPos(const cocos2d::Vec2& pos) = 0;
    virtual cc::Rect getBounds() = 0;
};