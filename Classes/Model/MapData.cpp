#include "Headers.h"

void MapData::extractInfo(const std::string &name)
{
    auto path = cc::FileUtils::getInstance()->fullPathForFilename(
        "maps/" + name + ".plist");
    auto rawData = cc::FileUtils::getInstance()->getValueMapFromFile(path);
    
    CCASSERT(rawData.find("layout") != rawData.end(), "invalid map data");
    auto layout = rawData.at("layout").asValueMap();
    depthConfig.first = layout.at("minDepth").asInt();
    depthConfig.second = layout.at("maxDepth").asInt();
    this->shape = new MapShape(layout.at("width").asInt(), layout.at("height").asInt());
    
    CCASSERT(rawData.find("rooms") != rawData.end(), "invalid map data");
    auto rooms = rawData.at("rooms").asValueMap();
    CCASSERT(rooms.find("content") != rooms.end(), "invalid map data");
    CCASSERT(rooms.find("start") != rooms.end(), "invalid map data");
    
    CCASSERT(rawData.find("background") != rawData.end(), "invalid map data");
    auto bgData = rawData.at("background").asValueMap();
    auto bgRawColor = std::vector<std::string>();
    lib::split(bgData.at("color").asString(), bgRawColor, ",", true);
    this->bgColor = Color3B(lib::parseInt(bgRawColor[0]),
                            lib::parseInt(bgRawColor[1]),
                            lib::parseInt(bgRawColor[2]));
    for(auto tile : bgData.at("tiles").asValueVector())
        this->bgTiles.push_back(tile.asString());
    
    for(auto contentModel : rooms.at("content").asValueVector())
    {
        this->addModel(RoomModel::create(contentModel.asString()));
    }
    
    for(auto contentModel : rooms.at("end").asValueVector())
    {
        this->addEndModel(RoomModel::create(contentModel.asString()));
    }
    
    //starter
    auto starterCount = rooms.at("start").asValueVector().size();
    auto starterModelName = rooms.at("start").asValueVector().at(rand() % starterCount).asString();
    this->startModel = RoomModel::create(starterModelName);
}

MapData::MapData(const std::string& fileName)
{
    this->shape = nullptr;
    this->extractInfo(fileName);
}

MapData* MapData::generate(const std::string& filename, int nbTry)
{
    auto seed = time(0);
    MapData* map = new MapData(filename);
    
    std::vector<RoomData*> roomList;
    
    std::vector<RoomModel*> modelList(map->getModels());
    std::vector<RoomModel*> endList(map->getEndModels());
    
    
    std::shuffle(modelList.begin(), modelList.end(),
                 std::default_random_engine(seed));
    
    std::shuffle(endList.begin(), endList.end(),
                 std::default_random_engine(seed));

    //starter
    auto firstRoom = map->addRoom({
        (unsigned)(map->shape->getSize().x / 2),
        (unsigned)(map->shape->getSize().y / 2)}, map->startModel);
    firstRoom->depth = 1;
    firstRoom->type = RoomData::RoomType::START;
    roomList.push_back(firstRoom);
    map->setCurIdxRoom(firstRoom->index);
    
    bool loop = true;
    bool withEndRoom = false;
    while(loop)
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
                RoomData::GateMap gateRef = pair.second;
                if (gateRef.roomIndex == 0) //gate free detected
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
        
        //let's try to place end room
        if (refRoom->depth >= map->depthConfig.second - 1 && !withEndRoom)
        {
            for(auto model : endList)
            {
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
                        withEndRoom = true;
                        
                        auto endRoom = map->addRoom(newPos, model);
                        endRoom->type = RoomData::RoomType::END;
                        roomList.push_back(endRoom);
                        
                        //link rooms
                        endRoom->gateMapping[destGateIdx] = {refRoom->index, refGateIdx};
                        refRoom->gateMapping[refGateIdx] = {endRoom->index, destGateIdx};
                        
                        //compute depth
                        map->computeDepth(endRoom);
                        break;
                    }
                }
                
                if (withEndRoom) break;
            }
            
            if (withEndRoom) continue;
        }
        
        RoomModel* modelFound = nullptr;
        for(auto model : modelList)
        {
            if (refRoom->depth <= map->depthConfig.first && model->gates.size() < 2)
                continue;
            if (refRoom->depth >= map->depthConfig.second - 1 && model->gates.size() > 1)
                continue;
                
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
                    
                    //compute depth
                    map->computeDepth(newRoom);
                    break;
                }
            }
        }
    
        if (modelFound == nullptr) //no one fit
        {
            loop = false;
        }
        else
        {
            modelList.erase(std::remove(modelList.begin(),
                                        modelList.end(),
                                        modelFound),
                            modelList.end());
            modelList.push_back(modelFound);
        }
    }
    
    if ((!loop || !withEndRoom) && nbTry < 50) //fail
    {
        delete map;
        return generate(filename, nbTry + 1);
    }
    
    CCASSERT(loop, "cannot generate map after 50 tries");
    CCLOG("nb try to generate the map = %d", nbTry);
    
    
    return map;
}

void MapData::computeDepth(RoomData *room)
{
    unsigned minDepth = 0;
    for(auto &pair : room->gateMapping)
    {
        RoomData::GateMap gateRef = pair.second;
        if (gateRef.roomIndex != 0) //gate filled
        {
            auto currentDepth = this->rooms[gateRef.roomIndex]->depth;
            if (minDepth == 0 || minDepth > currentDepth)
                minDepth = currentDepth;
        }
    }
    room->depth = minDepth + 1;
}

MapData::~MapData()
{
    if (this->startModel != nullptr)
        delete this->startModel;
    
    if (this->shape != nullptr)
        delete this->shape;
    
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
    for(auto model : this->endLib)
    {
        if (model != nullptr)
            delete model;
    }
}

void MapData::addModel(RoomModel *model)
{
    lib.push_back(model);
}

void MapData::addEndModel(RoomModel *model)
{
    endLib.push_back(model);
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
                if (this->shape->get(gridPos.x + i,
                                    gridPos.y + j) != MapShape::NONE)
                    return false;
            }
        }
    }
    
    //check gates
    for(auto pgate : model->shape.gates)
    {
        MapShape::BType btype = this->shape->get(gridPos.x + pgate.second.x,
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
        this->shape->fill({
            wRect.origin.x + gridPos.x - 1,
            wRect.origin.y + gridPos.y - 1,
            wRect.size.width + 2,
            wRect.size.height + 2
        });
    }
    for(auto gate : model->shape.gates)
    {
        lib::v2i gpos = gate.second;
        this->shape->get(gpos.x + gridPos.x,
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

unsigned MapData::getCurIdxRoom()
{
    return this->curIdxRoom;
}

RoomData* MapData::getRoomAt(unsigned int idx)
{
    CCASSERT(lib::hasKey(rooms, idx), "room is missing");
    return this->rooms[idx];
}

cc::Color3B MapData::getBgColor()
{
    return this->bgColor;
}

std::vector<std::string>& MapData::getBgTiles()
{
    return this->bgTiles;
}

const std::vector<RoomModel*> MapData::getModels()
{
    return this->lib;
}

const std::vector<RoomModel*> MapData::getEndModels()
{
    return this->endLib;
}

const std::set<std::string>& MapData::getSriteSheets()
{
    return this->spriteSheets;
}