#pragma once
#include "DataGrid.h"
#include "BlockInfo.h"
#include "GateInfo.h"
#include "V2.h"
#include "ObjectInfo.h"
#include "AIComponent.h"
#include "TmxDataModel.h"

class RoomModel : public TmxDataModel
{
public:
    static RoomModel* create(const std::string &fileName);
    
    RoomModel(const std::string &fileName) : TmxDataModel(fileName)
    {
        sleepZones[AIComponent::eSleep::BIRD] = std::list<cc::Rect>();
        sleepZones[AIComponent::eSleep::HUMAN] = std::list<cc::Rect>();
        
        this->initialize();
    }
    
    //areas for gate & warp
    std::list<GateInfo>                                         crossAreas;
    std::list<cc::Rect>                                         walls;
    
    //additional infos
    std::map<AIComponent::eSleep, std::list<cc::Rect>>    sleepZones;
protected:
    void initialize();
private:
    static GateInfo::GateType gessGateType(lib::v2u pos,
                                           const RoomModel& roomModel);
    static GateInfo extractCrossArea(RoomModel& roomData, lib::v2u coord);
};