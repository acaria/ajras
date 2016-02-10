#pragma once

#include "RoomModel.h"
#include "GateMap.h"
#include "CoreLib.h"
#include "AIComponent.h"
#include "IMapData.h"
#include "ISleepZones.h"
#include "CollisionInfo.h"
#include "NavigationInfo.h"
#include "Defines.h"

class RoomData : public IMapData, public ISleepZones
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
    
    //ctors
    RoomData(unsigned roomIndex, RoomModel* model);
    
    //overrides
    virtual lib::DataGrid<BlockInfo>& getGrid() override;
    virtual cc::Size getTileSize() override;
    virtual cc::Rect getBlockBound(const lib::v2u& coord) override;
    virtual cc::Rect getBounds() override;
    virtual lib::v2u getCoordFromPos(const cc::Vec2& pos) override;
    virtual CollisionInfo* getCol() override;
    virtual NavigationInfo* getNav() override;
    
    //getters
    lib::v2u                    getDim();
    lib::DataGrid<BlockInfo>&   getContent();
    std::vector<ObjectInfo>     getModelObjs();
    cc::Vec2                    getPosFromCoord(const lib::v2u& coord);
    std::list<cc::Rect>         getWalls();
    def::SleepZone*             getSleepZone(const std::string& cat,
                                             const cc::Point& pos) override;
    void freeSleepZone(const std::string& cat, const cc::Point& pos) override;
    
    std::pair<cc::Point, cc::Point> extractGateAnimInfo(const GateMap& gate, cc::Rect colRect);
    
    //fields
    unsigned        index;
    unsigned        depth;
    cocos2d::Vec2   position;
    RoomType        type;
    
    //mapping src_gateindex => dest_roomindex + dest_gateindex
    std::list<GateInfo>           crossAreas;
    std::map<unsigned, GateMap>   gateMapping;
    
    void init();
    GateMap getEnterGate();
    
private:
    RoomModel*                                          model;
    lib::DataGrid<BlockInfo>                            grid;
    std::map<std::string, std::list<def::SleepZone>>    sleepZones;
    CollisionInfo   collision;
    NavigationInfo  navigation;
};