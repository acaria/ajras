#pragma once
#include "Headers.h"

class RoomModel
{
public:
    static RoomModel* create(const std::string &fileName);
    
    RoomModel(lib::v2u dim, lib::v2u tileSize) :
        tileSize(tileSize),
        totalSize{dim.x * tileSize.x, dim.y * tileSize.y},
        grid(dim.x, dim.y) {}
    
    std::vector<std::string>     ss;
    std::vector<ObjectInfo>      objs;
    std::map<unsigned, GateInfo> gates;
    std::list<GateInfo>          warps;
    lib::DataGrid<BlockInfo>     grid;
    
    
    lib::v2u        tileSize; //sizeof a tile case (in pxl)
    lib::v2u        totalSize; //sizeof the room (in pxl)
    RoomShape       shape;
    std::string     name;
    
    cocos2d::Rect   getRectCoord(lib::v2u pos);
    cocos2d::Vec2   getPosCoord(lib::v2u pos);
    lib::v2u        getGridPos(cocos2d::Vec2 coord);
    int             getZOrder(const cocos2d::Vec2& pos);
    
private:
    static GateInfo::GateType gessGateType(lib::v2u pos, const RoomModel& mapData);
    static void processing(RoomModel& mapData);
    static void genShape(RoomModel& mapData);
    static GateInfo extractGate(RoomModel& mapData,
        const std::string& collisionType,
        unsigned i, unsigned j);
};