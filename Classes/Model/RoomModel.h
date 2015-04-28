#pragma once
#include "DataGrid.h"
#include "BlockInfo.h"
#include "GateInfo.h"
#include "V2.h"
#include "ObjectInfo.h"
#include "RoomShape.h"

class RoomModel
{
public:
    static RoomModel* create(const std::string &fileName);
    
    RoomModel(lib::v2u dim, lib::v2u tileSize) :
        tileSize(tileSize),
        totalSize{dim.x * tileSize.x, dim.y * tileSize.y},
        grid(dim.x, dim.y) {}

    std::vector<ObjectInfo>      objs;
    lib::DataGrid<BlockInfo>     grid;
    
    
    lib::v2u        tileSize; //sizeof a tile case (in pxl)
    lib::v2u        totalSize; //sizeof the room (in pxl)
    //RoomShape       shape;
    std::string     name;
    
    cc::Rect        getRectCoord(lib::v2u pos);
    cc::Vec2        getPosFromCoord(lib::v2u pos);
    lib::v2u        getCoordFromPos(cocos2d::Vec2 coord);
    int             getZOrder(const cocos2d::Vec2& pos);
    
    //declare wall shapes
    std::list<cc::Rect> walls;
    //areas for gate & warp
    std::list<GateInfo> crossAreas;
    /*void            extractGateringIntroInfo(unsigned gateIndex,
                                             cc::Rect colRect,
                                             cc::Point& srcPos,
                                             cc::Point& destPos);*/
    
private:
    static GateInfo::GateType gessGateType(lib::v2u pos,
                                           const RoomModel& roomModel);
    //static void genShape(RoomModel& mapData);
    static GateInfo extractCrossArea(RoomModel& mapData, lib::v2u coord);
};