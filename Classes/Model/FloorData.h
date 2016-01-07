#pragma once
class RoomModel;

#include "V2.h"
#include "GateInfo.h"
#include "RoomData.h"
#include "FloorMapping.h"
#include "Random.h"
#include "Defines.h"

using GateConfig = std::map<GateInfo::GateType, std::pair<std::string, cc::Rect>>;
using ModelMap = std::map<std::string, RoomModel*>;
using ModelVector = std::vector<RoomModel*>;

class FloorData
{
public:
    static FloorData* generate(const std::string& fileName);

    FloorData(const std::string& fileName);
    ~FloorData();
    
    RoomData* addRoom(RoomModel* data, const RoomData::Config& config);
    
    RoomData*                       getRoomAt(unsigned idx);
    RoomData*                       getCurrentRoom();
    const std::set<std::string>&    getSpriteSheets();
    std::map<unsigned, RoomData*>   rooms;
    
    FloorMapping*                   floorMapping;
    
    const ModelVector               getModels(const std::string& cat);
    
    PROPERTY(unsigned, curIdxRoom, CurIdxRoom);
    PROPERTY_READ(cc::Color3B, bgColor, BgColor);
    PROPERTY_READ(std::vector<std::string>, bgTiles, BgTiles);
    PROPERTY_READ(def::shader::LightConfig, lightConfig, LightConfig);
    
private:
    void                            extractInfo(const std::string& name);
    
    ModelMap                            modelLib;
    std::map<std::string, ModelVector>  modelCategory;
    
    static RoomModel*               pickModel(ModelVector& modelList);
    
    std::set<std::string>           spriteSheets;
    void                            computeDepth(RoomData* room);
    
    std::pair<unsigned, unsigned>   depthConfig;
    std::pair<unsigned, unsigned>   sizeConfig;
    std::pair<unsigned, unsigned>   startNbGates;
    
    GateConfig                      gateConfig;
    GateConfig                      warpConfig;
    
    lib::Random&                    random;
    
    std::pair<std::string, cc::Rect> subExtractGateInfo(const cc::ValueMap& el);
};