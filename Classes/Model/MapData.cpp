#include "MapData.h"
#include "FloorMapping.h"
#include "RoomModel.h"
#include "RoomData.h"
#include "GateMap.h"
#include "Randgine.h"

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
    
    this->floorMapping = new FloorMapping(
        layout.at("width").asInt(), layout.at("height").asInt(),
        this->gateConfig);
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

MapData::MapData(const std::string& fileName) :
    random(Randgine::instance()->get(Randgine::MAP))
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

MapData* MapData::generate(const std::string& filename)
{
    MapData* map = new MapData(filename);
    
    std::map<std::string, ModelVector>  modelList;
    
    for(auto modelCat : {"common", "empty"})
    {
        modelList[modelCat] = ModelVector(map->getModels(modelCat));
        std::shuffle(modelList[modelCat].begin(),
                     modelList[modelCat].end(),
                     map->random.getEngine());
    }

    //starter
    auto firstModel = MapData::pickModel(modelList["empty"]);
    auto firstRoom = map->addRoom(firstModel, {
        .pos = map->floorMapping->getStartPosition(firstModel),
        .nbGates = map->startNbGates,
        .profile = RoomData::RoomType::STARTER
    });
    firstRoom->depth = 1;
    map->setCurIdxRoom(firstRoom->index);
    
    bool endRoomPlaced = false;
    while(map->floorMapping->withCrossingLeft())
    {
        bool found = false;
        auto& crossingInfo = map->floorMapping->selectCrossing();
        
        auto currentDepth = map->rooms[crossingInfo.refRoomIndex]->depth;
        
        auto targetProfile = RoomData::RoomType::COMMON;
        auto targetModel = "common";
        auto nbGates = std::pair<unsigned, unsigned>(1, 1);
        
        if (currentDepth < map->depthConfig.first - 1)
        {
            nbGates = {2, 4};
        }
        else if (currentDepth < map->depthConfig.second - 1)
        {
            if (!endRoomPlaced)
            {
                targetProfile = RoomData::RoomType::FINISHER;
                endRoomPlaced = true;
                targetModel = "empty";
            }
            else
            {
                nbGates = {1, 3};
            }
        }
    
        for(auto model : modelList["common"])
        {
            for(auto gateInfo : model->crossAreas)
            {
                if (gateInfo.type == crossingInfo.gateInfo.type) //gate match
                {
                    //processing all possible positions
                    auto positions = map->floorMapping->extractPositions(model, crossingInfo.gateInfo, gateInfo);
                    
                    if (positions.size() > 0) //room
                    {
                        auto room = map->addRoom(model, {
                            .pos = map->random.select(positions),
                            .nbGates = nbGates,
                            .profile = targetProfile,
                        });
                        if (room != nullptr)
                        {
                            map->computeDepth(room);
                            found = true;
                            break;
                        }
                        else
                        {
                           Log("mapgen: invalid room, cannot generate map");
                        }
                    }
                }
            }
            
            if (found)
                break;
        }
        
        if (!found)
        {
            //let's try another crossingInfo
            bool crossingChanged = false;
            auto refRoom = map->rooms[crossingInfo.refRoomIndex];
            
            while (refRoom->crossAreas.size() > 0)
            {
                auto area = map->random.pick(refRoom->crossAreas);
                if (map->floorMapping->tryChangeCross(/*ref*/crossingInfo,
                                                      refRoom->position, area))
                {
                    crossingChanged = true;
                    break;
                }
            }
            
            if (!crossingChanged)
            {
                if (refRoom->gateMapping.size() < 1)
                {
                    Log("mapgen: invalid map, cannot find correct room position");
                    break;
                }
                else
                {
                    map->floorMapping->removeCrossing(crossingInfo);
                }
            }
        }
    }
    
    if (!endRoomPlaced)
    {
        Log("mapgen: end room is missing");
    }
    
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
    if (room == nullptr)
        return;
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

RoomData* MapData::addRoom(RoomModel* model, const RoomData::Config& config)
{
    static unsigned roomIndex = 1;
    auto room = new RoomData(roomIndex++, model);
    room->type = config.profile;
    room->position = config.pos;

    auto nbGates = random.interval(config.nbGates.first,
                                   config.nbGates.second);
    nbGates = lib::clamp((int)nbGates, 1, (int)room->crossAreas.size());
    CCASSERT(room->crossAreas.size() >= nbGates, "too small");
    
    unsigned gateIndex = 1;
    
    //gates binder
    auto gates = this->floorMapping->bindGates(config.pos, room, /*ref*/gateIndex);
    
    if (gates.size() == 0 &&
        config.profile != RoomData::RoomType::STARTER)
    {
        //error
        delete room;
        return nullptr;
    }
    
    for(GateMap gate : gates)
    {
        auto gIndex = room->gateMapping[gate.destGateIndex].destGateIndex;
        auto rIndex = room->gateMapping[gate.destGateIndex].destRoomIndex;
        auto destRoom = this->rooms[rIndex];
        gate.info.rect.origin -= destRoom->position;
        destRoom->gateMapping[gIndex] = gate;
        nbGates--;
    }
    
    //handle special gates (warps)
    if (config.profile == RoomData::RoomType::STARTER ||
        config.profile == RoomData::RoomType::FINISHER)
    {
        GateMap gateMap = this->floorMapping->createSpecialGate(
            room, config.profile, this->warpConfig);
        room->gateMapping[gateIndex++] = gateMap;
        nbGates--;
    }
    
    //door gates
    int i = nbGates;
    while(i > 0 && room->crossAreas.size() > 0)
    {
        auto crossInfo = random.pick(room->crossAreas);
        if (this->floorMapping->tryAddCross(room->position,
            crossInfo, room->index, gateIndex++))
        {
            i--;
        }
    }
    
    this->floorMapping->addRoom(room->position, model->walls);
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