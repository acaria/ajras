#pragma once
class RoomModel;
class FloorMapping;

#include "V2.h"
#include "GateInfo.h"
#include "RoomData.h"

using GateConfig = std::map<GateInfo::GateType, std::pair<std::string, cc::Rect>>;
using ModelMap = std::map<std::string, RoomModel*>;
using ModelVector = std::vector<RoomModel*>;

class MapData
{
public:
    static MapData* generate(const std::string& fileName,
                             long seed,
                             int nbTry = 50);

    MapData(const std::string& fileName);
    ~MapData();
    
    RoomData* addRoom(RoomModel* data, RoomData::Config config);
    void setCurIdxRoom(unsigned roomIndex);
    
    RoomData*                       getRoomAt(unsigned idx);
    const std::set<std::string>&    getSriteSheets();
    std::map<unsigned, RoomData*>   rooms;
    
    FloorMapping*                   floorMapping;
    
    const ModelVector               getModels(const std::string& cat);
    unsigned                        getCurIdxRoom();
    cc::Color3B                     getBgColor();
    std::vector<std::string>&       getBgTiles();
    
private:
    void                            extractInfo(const std::string& name);
    unsigned                        curIdxRoom;
    
    ModelMap                            modelLib;
    std::map<std::string, ModelVector>  modelCategory;
    
    static RoomModel*               pickModel(ModelVector& modelList);
    
    std::set<std::string>           spriteSheets;
    void                            computeDepth(RoomData* room);
    
    cc::Color3B                     bgColor;
    std::vector<std::string>        bgTiles;
    
    std::pair<unsigned, unsigned>   depthConfig;
    std::pair<unsigned, unsigned>   sizeConfig;
    std::pair<unsigned, unsigned>   startNbGates;
    
    GateConfig                      gateConfig;
    GateConfig                      warpConfig;
    
    bool checkInsertRoom(lib::v2u gridPos, RoomModel* model);
    std::pair<std::string, cc::Rect> subExtractGateInfo(const cc::ValueMap& el);
};