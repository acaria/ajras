#pragma once
#include "GateInfo.h"
#include "RoomData.h"
#include "Randgine.h"
#include "Misc.h"

class FloorMapping
{
public:
    const float margin = 6;

    //defines
    struct CrossingInfo
    {
        unsigned refRoomIndex;
        unsigned refGateIndex;
        GateInfo gateInfo;
    };

    //ctors
    FloorMapping(unsigned width, unsigned height, std::map<GateInfo::GateType,
                 std::pair<std::string, cc::Rect>> gateConfig);
    
    void addRoom(cc::Point roomPos, std::list<cc::Rect> walls);
    
    cc::Point getStartPosition(RoomModel* model);
    CrossingInfo& selectCrossing();
    void removeCrossing(const CrossingInfo& info);
    
    std::list<cc::Point> extractPositions(RoomModel* model,
                                          GateInfo gateAbs,
                                          GateInfo gateRel);
    GateMap createSpecialGate(RoomData* room,
        RoomData::RoomType roomType,
        std::map<GateInfo::GateType, std::pair<std::string, cc::Rect>> gateConfig);
    std::list<GateMap> bindGates(cc::Point pos,
                                 RoomData* roomData,
                                 unsigned& gateIndex);

    bool withCrossingLeft();
    bool tryAddCross(cc::Point roomPos,
                  GateInfo crossing,
                  unsigned roomIndex,
                  unsigned gateIndex);
    bool tryChangeCross(CrossingInfo& crossingInfo,
                        cc::Point roomPos,
                        GateInfo newGateInfo);
    
    cc::Size getMappingSize();
    
private:
    //components
    lib::Random                   random;

    //internal methods
    GateInfo getOppositeFromGateInfo(GateInfo info);
    bool checkRoom(cc::Point roomPos, RoomModel* model);

    //fields
    cc::Size size;
    std::map<GateInfo::GateType, std::pair<std::string, cc::Rect>> gateConfig;
    std::vector<cc::Rect>         takenList;
    cc::Rect                      totalBounds;
    std::vector<CrossingInfo>     crossingLeft;
};