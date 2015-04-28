#include "MapData.h"
#include "FloorMapping.h"
#include "RoomModel.h"
#include "RoomData.h"
#include "GateMap.h"

void MapData::extractInfo(const std::string &name)
{
    auto path = cc::FileUtils::getInstance()->fullPathForFilename(
        "maps/" + name + ".plist");
    auto rawData = cc::FileUtils::getInstance()->getValueMapFromFile(path);
    
    //layout
    CCASSERT(rawData.find("layout") != rawData.end(), "invalid map data");
    auto layout = rawData.at("layout").asValueMap();
    depthConfig.first = layout.at("min_depth").asInt();
    depthConfig.second = layout.at("max_depth").asInt();
    this->floorMapping = new FloorMapping(layout.at("width").asInt(), layout.at("height").asInt());
    auto minMaxStartNbGates = std::vector<std::string>();
    lib::split(layout.at("start_nb_gates").asString(), minMaxStartNbGates, ",", true);
    this->startNbGates.first = lib::parseInt(minMaxStartNbGates[0]);
    this->startNbGates.second = lib::parseInt(minMaxStartNbGates[1]);
    
    //design
    CCASSERT(rawData.find("design") != rawData.end(), "invalid map data");
    auto design = rawData.at("design").asValueMap();
    auto bgRawColor = std::vector<std::string>();
    lib::split(design.at("bg_color").asString(), bgRawColor, ",", true);
    this->bgColor = cc::Color3B(lib::parseInt(bgRawColor[0]),
                            lib::parseInt(bgRawColor[1]),
                            lib::parseInt(bgRawColor[2]));
    for(auto tile : design.at("bg_tiles").asValueVector())
        this->bgTiles.push_back(tile.asString());
    CCASSERT(design.find("gate") != design.end(), "invalid gate data");
    for(auto element : design.at("gate").asValueMap())
    {
        this->gateConfig[GateInfo::typeFromStr(element.first)] = this->subExtractGateInfo(element.second.asValueMap());
    }
    CCASSERT(design.find("warp") != design.end(), "invalid warp data");
    for(auto element : design.at("warp").asValueMap())
    {
        this->warpConfig[GateInfo::typeFromStr(element.first)] = this->subExtractGateInfo(element.second.asValueMap());
    }
    CCASSERT(design.find("ss") != design.end(), "invalid ss data");
    for(auto ss : design.at("ss").asValueVector())
    {
        this->spriteSheets.insert(ss.asString());
    }
    
    //rooms
    CCASSERT(rawData.find("rooms") != rawData.end(), "invalid room data");
    auto rooms = rawData.at("rooms").asValueMap();
    for(auto roomCategory : {"common", "empty"})
    {
        CCASSERT(rooms.find(roomCategory) != rooms.end(),
                 "invalid room category data");
        for(auto contentModel : rooms.at(roomCategory).asValueVector())
        {
            auto modelName = contentModel.asString();
            if (!lib::hasKey(this->modelLib, modelName))
                this->modelLib[modelName] = RoomModel::create(modelName);
            if (!lib::hasKey(this->modelCategory, roomCategory))
                this->modelCategory[roomCategory] = ModelVector();
            this->modelCategory[roomCategory].push_back(modelLib[modelName]);
        }
    }
}

std::pair<std::string, cc::Rect> MapData::subExtractGateInfo(const cc::ValueMap& el)
{
    CCASSERT(el.find("tile") != el.end(), "invalid gate info");
    CCASSERT(el.find("bounds") != el.end(), "invalid gate info");
    
    auto tileName = el.at("tile").asString();
    auto tileBounds = std::vector<std::string>();
    lib::split(el.at("bounds").asString(), tileBounds, ",", true);

    cc::Rect gateBounds = cc::Rect(
        lib::parseFloat(tileBounds[0]),
        lib::parseFloat(tileBounds[1]),
        lib::parseFloat(tileBounds[2]),
        lib::parseFloat(tileBounds[3])
    );
    
    return {tileName, gateBounds};
}

MapData::MapData(const std::string& fileName)
{
    this->floorMapping = nullptr;
    this->extractInfo(fileName);
}

const ModelVector MapData::getModels(const std::string& category)
{
    if (!lib::hasKey(this->modelCategory, category))
        return ModelVector();
    return this->modelCategory[category];
}

MapData* MapData::generate(const std::string& filename, long seed, int nbTry)
{
    MapData* map = new MapData(filename);
    
    std::vector<RoomData*>              roomList;
    std::map<std::string, ModelVector>  modelList;
    
    for(auto modelCat : {"common", "empty"})
    {
        modelList[modelCat] = ModelVector(map->getModels(modelCat));
        std::shuffle(modelList[modelCat].begin(),
                     modelList[modelCat].end(),
                     std::default_random_engine(seed));
    }

    //starter
    auto firstModel = MapData::pickModel(modelList["empty"]);
    auto coord = lib::v2u(
        (map->floorMapping->getSize().x / 2),
        (map->floorMapping->getSize().y / 2)
    );
    
    auto firstRoom = map->addRoom(firstModel, {
        .coord = coord,
        .nbGates = map->startNbGates,
        .profile = RoomData::RoomType::STARTER
    });
    firstRoom->depth = 1;
    
    roomList.push_back(firstRoom);
    map->setCurIdxRoom(firstRoom->index);
    
    bool loop = true;
    bool withEndRoom = false;
    /*while(loop)
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
            MapData::sendBackModel(modelList, modelFound);
        }
    }
    
    if ((!loop || !withEndRoom) && nbTry > 0) //fail
    {
        delete map;
        return generate(filename, nbTry - 1);
    }
    
    CCASSERT(loop, "cannot generate map after 50 tries");
    CCLOG("nb try to generate the map = %d", nbTry);
    */
    
    return map;
}

RoomModel* MapData::pickModel(ModelVector& modelList)
{
    auto model = modelList.front();
    modelList.erase(std::remove(modelList.begin(), modelList.end(), model),
                    modelList.end());
    modelList.push_back(model);
    return model;
}

void MapData::computeDepth(RoomData *room)
{
    unsigned minDepth = 0;
    for(auto &pair : room->gateMapping)
    {
        GateMap gateRef = pair.second;
        if (gateRef.destRoomIndex != 0) //gate filled
        {
            auto currentDepth = this->rooms[gateRef.destRoomIndex]->depth;
            if (minDepth == 0 || minDepth > currentDepth)
                minDepth = currentDepth;
        }
    }
    room->depth = minDepth + 1;
}

MapData::~MapData()
{
    //map shape
    if (this->floorMapping != nullptr)
        delete this->floorMapping;
    
    //clear rooms
    for(auto room : this->rooms)
    {
        if (room.second != nullptr)
            delete room.second;
    }
    
    //clear models
    for(auto model : this->modelLib)
    {
        if (model.second != nullptr)
            delete model.second;
    }
}

bool MapData::checkInsertRoom(lib::v2u gridPos, RoomModel* model)
{
    /*
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
    }*/
    
    return true;
}

RoomData* MapData::addRoom(RoomModel* model, RoomData::Config config)
{
    static unsigned roomIndex = 1;
    auto room = new RoomData(roomIndex++, model);
    room->type = config.profile;
    room->position = {(float)config.coord.x * model->tileSize.x,
                      (float)config.coord.y * model->tileSize.y};
    auto nbGates = lib::randAB(config.nbGates.first, config.nbGates.second);
    
    CCASSERT(room->crossAreas.size() >= nbGates, "too small");
    
    int gateIndex = 1;
    
    bool ifStarter = false;
    bool ifFinisher = false;
    
    for(int i = nbGates; i > 0; i--)
    {
        auto crossInfoIt = lib::selectRand(room->crossAreas.begin(),
                                          room->crossAreas.end());
        auto crossInfo = *crossInfoIt;
        room->crossAreas.erase(crossInfoIt);
        
        cc::Rect gateRect = this->gateConfig[crossInfo.type].second;
        
        int tx = (crossInfo.rect.size.width - gateRect.size.width) / model->tileSize.x;
        int ty = (crossInfo.rect.size.height - gateRect.size.height) / model->tileSize.y;
        
        GateMap gateMap;
        gateMap.info.type = crossInfo.type;
        gateMap.info.rect = cc::Rect(
            crossInfo.rect.origin.x + (model->tileSize.x * lib::randAB(0, tx)),
            crossInfo.rect.origin.y + (model->tileSize.y * lib::randAB(0, ty)),
            gateRect.size.width, gateRect.size.height
        );
        
        if (config.profile == RoomData::RoomType::STARTER && !ifStarter)
        {
            ifStarter = true;
            gateMap.cmd = GateMap::CmdType::ENTER_MAP;
            gateMap.tileName = this->warpConfig[crossInfo.type].first;
        }
        else if (config.profile == RoomData::RoomType::FINISHER && !ifFinisher)
        {
            ifFinisher = true;
            gateMap.cmd = GateMap::CmdType::EXIT_MAP;
            gateMap.tileName = this->warpConfig[crossInfo.type].first;
        }
        else
        {
            gateMap.cmd = GateMap::CmdType::CHANGE_ROOM;
            gateMap.tileName = this->gateConfig[crossInfo.type].first;
        }
        
        room->gateMapping[gateIndex++] = gateMap;
    }
    
    /*for(auto wRect : model->shape.walls)
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
    }*/
    
    rooms[room->index] = room;
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

const std::set<std::string>& MapData::getSriteSheets()
{
    return this->spriteSheets;
}