#pragma once

#include "RoomModel.h"
#include "GateMap.h"
#include "DataGrid.h"
#include "V2.h"
#include "AIComponent.h"
#include "IMapData.h"

class RoomData : public IMapData
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
    
    //overrides
    virtual lib::DataGrid<BlockInfo>& getGrid() override;
    virtual cc::Size getTileSize() override;
    virtual int getZOrder(const cc::Vec2& pos) override;
    virtual cc::Rect getBlockBound(lib::v2u coord) override;
    virtual cc::Rect getBounds() override;
    
    //getters
    lib::v2u                    getDim();
    lib::DataGrid<BlockInfo>&   getContent();
    std::vector<ObjectInfo>     getModelObjs();
    cc::Vec2                    getPosFromCoord(const lib::v2u& coord);
    lib::v2u                    getCoordFromPos(const cc::Vec2& pos);
    std::list<cc::Rect>         getWalls();
    SleepZone*                  getSleepZone(AIComponent::eSleep cat,
                                             const cc::Point& pos);
    void freeSleepZone(AIComponent::eSleep cat, const cc::Point& pos);
    
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
    std::map<AIComponent::eSleep, std::list<SleepZone>> sleepZones;
};