#pragma once
#include "Headers.h"

class MapData
{
public:
    static MapData* generate();

    MapData() {}
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
    unsigned                        curIdxRoom;
    std::vector<RoomModel*>         lib;
    std::set<std::string>           spriteSheets;
    bool checkInsertRoom(lib::v2u gridPos, RoomModel* model);
};