#include "Headers.h"

MapData* MapData::generate()
{
    auto seed = time(0);
    MapData* map = new MapData();
    
    map->addModel(RoomModel::create("map1r1"));
    map->addModel(RoomModel::create("map1r2"));
    map->addModel(RoomModel::create("map1r3"));
    map->addModel(RoomModel::create("map1r4"));
    
    std::vector<RoomModel*> modelList(map->getModels());
    std::vector<RoomData*> roomList;
    
    std::shuffle(modelList.begin(), modelList.end(),
                 std::default_random_engine(seed));
    
    //starter
    auto starterModel = RoomModel::create("map1s1");
    map->addModel(starterModel);
    auto firstRoom = map->addRoom({
        (unsigned)(map->shape.getSize().x / 2),
        (unsigned)(map->shape.getSize().y / 2)}, starterModel);
    roomList.push_back(firstRoom);
    map->setCurIdxRoom(firstRoom->index);
    
    while(modelList.size() > 0)
    {
        std::shuffle(roomList.begin(), roomList.end(),
            std::default_random_engine(seed));
            
        RoomData* refRoom = nullptr;
        GateInfo* refGateInfo = nullptr;
        unsigned  refGateIdx = 0;
        for(auto room : roomList)
        {
            for(auto &pair : room->gateMapping)
            {
                auto gateRef = pair.second;
                if (gateRef.first == 0) //gate free detected
                {
                    refGateInfo = &room->getModel()->gates[pair.first];
                    refGateIdx = pair.first;
                    refRoom = room;
                    break;
                }
                if (refRoom != nullptr)
                    break;
            }
        }
        
        if (refRoom == nullptr) //finished map
        {
            break;
        }
        
        //room + gate selected, find and link another room
        lib::v2u refRoomPos {
            (unsigned)(refRoom->position.x / refRoom->getModel()->tileSize.x),
            (unsigned)(refRoom->position.y / refRoom->getModel()->tileSize.y)
        };
        lib::v2i refGatePos {
            refRoom->getModel()->shape.gates[refGateIdx].x,
            refRoom->getModel()->shape.gates[refGateIdx].y
        };
        
        unsigned destGateIdx = 0;
            
        GateInfo::GateType searchType = refGateInfo->getOpposite();
        RoomModel* modelFound = nullptr;
        for(auto model : modelList)
        {
            if (modelFound) break;
        
            for(auto pModelGate: model->gates)
            {
                destGateIdx = pModelGate.first;
                if (pModelGate.second.type != searchType)
                    continue; //wrong gate type
                
                lib::v2u newPos = {
                    refRoomPos.x + refGatePos.x - model->shape.gates[pModelGate.first].x,
                    refRoomPos.y + refGatePos.y - model->shape.gates[pModelGate.first].y
                };
                    
                if (map->checkInsertRoom(newPos, model))
                {
                    modelFound = model;
                    auto newRoom = map->addRoom(newPos, model);
                    roomList.push_back(newRoom);
                
                    //link rooms
                    newRoom->gateMapping[destGateIdx] = {refRoom->index, refGateIdx};
                    refRoom->gateMapping[refGateIdx] = {newRoom->index, destGateIdx};
                    break;
                }
            }
        }
    
        if (modelFound == nullptr) //no one fit
            modelList.clear(); //fail
        else
            modelList.erase(std::remove(modelList.begin(), modelList.end(), modelFound),
                            modelList.end());
    }
    
    return map;
}

MapData::~MapData()
{
    //clearrooms
    for(auto room : this->rooms)
    {
        if (room.second != nullptr)
            delete room.second;
    }
    
    //clear models
    for(auto model : this->lib)
    {
        if (model != nullptr)
            delete model;
    }
}

void MapData::addModel(RoomModel *model)
{
    lib.push_back(model);
}

bool MapData::checkInsertRoom(lib::v2u gridPos, RoomModel* model)
{
    //check walls
    for(auto wall : model->shape.walls)
    {
        for(int j=0; j < wall.size.height; j++)
        {
            for(int i=0; i < wall.size.width; i++)
            {
                if (this->shape.get(gridPos.x + i,
                                    gridPos.y + j) != MapShape::NONE)
                    return false;
            }
        }
    }
    
    //check gates
    for(auto pgate : model->shape.gates)
    {
        MapShape::BType btype = this->shape.get(gridPos.x + pgate.second.x,
                                                gridPos.y + pgate.second.y);
        if (btype == MapShape::WALL)
            return false;
    }
    
    return true;
}

RoomData* MapData::addRoom(lib::v2u gridPos, RoomModel* model)
{
    static unsigned roomIndex = 1;
    auto room = new RoomData(roomIndex++, model);
    room->position = {(float)gridPos.x * model->tileSize.x,
                      (float)gridPos.y * model->tileSize.y};
    for(auto wRect : model->shape.walls)
    {
        this->shape.fill({
            wRect.origin.x + gridPos.x - 1,
            wRect.origin.y + gridPos.y - 1,
            wRect.size.width + 2,
            wRect.size.height + 2
        }, MapShape::WALL);
    }
    for(auto gate : model->shape.gates)
    {
        lib::v2i gpos = gate.second;
        this->shape.get(gpos.x + gridPos.x,
                        gpos.y + gridPos.y) = MapShape::GATE;
    }

    for (auto ssName : model->ss)
        this->spriteSheets.insert(ssName);
    
    rooms.insert(std::make_pair(room->index, room));
    return room;
}

void MapData::setCurIdxRoom(unsigned int roomIndex)
{
    CCASSERT(lib::hasKey(rooms, roomIndex), "room is missing");
    this->curIdxRoom = roomIndex;
}

RoomData* MapData::getRoomAt(unsigned int idx)
{
    CCASSERT(lib::hasKey(rooms, idx), "room is missing");
    return this->rooms[idx];
}

const std::vector<RoomModel*> MapData::getModels()
{
    return this->lib;
}

RoomData* MapData::getCurRoom()
{
    CCASSERT(lib::hasKey(rooms, this->curIdxRoom), "room is missing");
    return this->rooms[this->curIdxRoom];
}

const std::set<std::string>& MapData::getSriteSheets()
{
    return this->spriteSheets;
}
