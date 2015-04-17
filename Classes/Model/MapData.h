#pragma once
class RoomModel;
class RoomData;
class MapShape;

#include "V2.h"

class MapData
{
public:
    static MapData* generate(const std::string& fileName,
                             int nbTry = 1);

    MapData(const std::string& fileName);
    ~MapData();
    
    RoomData* addRoom(lib::v2u gridPos, RoomModel* data);
    void addModel(RoomModel* model);
    void addEndModel(RoomModel* model);
    void setCurIdxRoom(unsigned roomIndex);
    
    RoomData*                       getRoomAt(unsigned idx);
    const std::vector<RoomModel*>   getModels();
    const std::vector<RoomModel*>   getEndModels();
    const std::set<std::string>&    getSriteSheets();
    std::map<unsigned, RoomData*>   rooms;
    
    MapShape*                       shape;
    
    unsigned                        getCurIdxRoom();
    cc::Color3B                     getBgColor();
    std::vector<std::string>&       getBgTiles();
    
private:
    void                            extractInfo(const std::string& name);
    unsigned                        curIdxRoom;
    
    std::vector<RoomModel*>         lib;
    RoomModel*                      startModel;
    std::vector<RoomModel*>         endLib;
    std::set<std::string>           spriteSheets;
    void                            computeDepth(RoomData* room);
    
    cc::Color3B                     bgColor;
    std::vector<std::string>        bgTiles;
    
    std::pair<unsigned, unsigned>   depthConfig;
    std::pair<unsigned, unsigned>   sizeConfig;
    
    bool checkInsertRoom(lib::v2u gridPos, RoomModel* model);
};