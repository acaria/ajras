#pragma once
#include "Headers.h"

class MapData
{
public:
    static MapData* generate(const std::string& fileName);

    MapData(const std::string& fileName);
    ~MapData();
    
    RoomData* addRoom(lib::v2u gridPos, RoomModel* data);
    void addModel(RoomModel* model);
    void setCurIdxRoom(unsigned roomIndex);
    
    unsigned                        getStartRoomIdx();
    RoomData*                       getRoomAt(unsigned idx);
    std::vector<RoomData*>          getRandRoom();
    const std::vector<RoomModel*>   getModels();
    const std::set<std::string>&    getSriteSheets();
    std::map<unsigned, RoomData*>   rooms;
    MapShape                        shape;
    
private:
    void                            extractInfo(const std::string& name);
    unsigned                        curIdxRoom;
    std::vector<RoomModel*>         lib;
    RoomModel*                      startModel;
    std::set<std::string>           spriteSheets;
    
    cc::Color3B                     bgColor;
    std::vector<std::string>        bgTiles;
    
    bool checkInsertRoom(lib::v2u gridPos, RoomModel* model);
};