#pragma once

#include "RoomModel.h"
#include "GateMap.h"
#include "DataGrid.h"
#include "V2.h"
#include "CategoryComponent.h"

class RoomData
{
public:

    //definitions
    enum class RoomType
    {
        STARTER,
        COMMON,
        FINISHER
    };
    
    struct Config
    {
        cc::Point                       pos;
        std::pair<unsigned, unsigned>   nbGates;
        RoomType                        profile;
    };
    
    struct SleepZone
    {
        cc::Rect    bounds;
        bool        taken;
    };
    
    //ctors
    RoomData(unsigned roomIndex, RoomModel* model);
    
    //getters
    cc::Rect                    getBounds();
    lib::v2u                    getDim();
    lib::DataGrid<BlockInfo>&   getContent();
    std::vector<ObjectInfo>     getModelObjs();
    int                         getZOrder(const cc::Vec2& pos);
    cc::Vec2                    getPosFromCoord(const lib::v2u& coord);
    lib::v2u                    getCoordFromPos(const cc::Vec2& pos);
    cc::Rect                    getBlockBound(lib::v2u coord);
    std::list<cc::Rect>         getWalls();
    SleepZone*                  getSleepZone(CategoryComponent::eSleep cat,
                                             const cc::Point& pos);
    void freeSleepZone(CategoryComponent::eSleep cat, const cc::Point& pos);
    
    void extractGateAnimInfo(unsigned gateIndex, cc::Rect colRect,
                             cc::Point& srcPos, cc::Point& destPos);
    
    //fields
    unsigned        index;
    unsigned        depth;
    cocos2d::Vec2   position;
    RoomType        type;
    
    //mapping src_gateindex => dest_roomindex + dest_gateindex
    std::list<GateInfo>           crossAreas;
    std::map<unsigned, GateMap>   gateMapping;
    
    
private:
    RoomModel*                            model;
    lib::DataGrid<BlockInfo>              grid;
    std::map<CategoryComponent::eSleep,
             std::list<SleepZone>>        sleepZones;
};