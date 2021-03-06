#include "FloorData.h"
#include "FloorMapping.h"
#include "RoomModel.h"
#include "RoomData.h"
#include "GateMap.h"
#include "Randgine.h"

void FloorData::extractInfo(const std::string &name)
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
    this->startNbGates.first = std::stoi(minMaxStartNbGates[0]);
    this->startNbGates.second = std::stoi(minMaxStartNbGates[1]);
    
    //effect
    if (rawData.find("effect") != rawData.end())
    {
        auto effect = rawData.at("effect").asValueMap();
        if (effect.find("light") != effect.end())
        {
            auto light = effect.at("light").asValueMap();
            LightConfig lightConfig;
            
            if (light.find("spot") != light.end())
            {
                auto spot = light.at("spot").asValueMap();
                
                auto lightColor = std::vector<std::string>();
                lib::split(spot.at("color").asString(), lightColor, ",", true);
                lightConfig.spot.color = cc::Color3B(std::stoi(lightColor[0]),
                                                     std::stoi(lightColor[1]),
                                                     std::stoi(lightColor[2]));
                lightConfig.spot.brightness = spot.at("brightness").asFloat();
                lightConfig.spot.cutOffRadius = spot.at("cutOffRadius").asFloat();
                lightConfig.spot.halfRadius = spot.at("halfRadius").asFloat();
                lightConfig.spot.pos.z = spot.at("depth").asFloat();
            }
            
            if (light.find("ambiantColor") != light.end())
            {
                auto ambiantColor = std::vector<std::string>();
                lib::split(light.at("ambiantColor").asString(), ambiantColor, ",", true);
                lightConfig.ambiantColor = cc::Color3B(std::stoi(ambiantColor[0]),
                                                       std::stoi(ambiantColor[1]),
                                                       std::stoi(ambiantColor[2]));
            }
            
            if (light.find("object") != light.end())
            {
                auto obj = light.at("object").asValueMap();
                for(auto el : obj)
                {
                    auto info = el.second.asValueMap();
                    
                    auto lightColor = std::vector<std::string>();
                    lib::split(info.at("color").asString(), lightColor, ",|", true);
                    
                    lightConfig.objects[el.first] = LightConfig::ObjectInfo();
                    
                    if (lightColor.size() == 6)
                    {
                        lightConfig.objects[el.first].color = {
                            cc::Color3B(std::stoi(lightColor[0]),
                                        std::stoi(lightColor[1]),
                                        std::stoi(lightColor[2])),
                            cc::Color3B(std::stoi(lightColor[3]),
                                        std::stoi(lightColor[4]),
                                        std::stoi(lightColor[5]))
                        };
                    }
                    else
                    {
                        assert(lightColor.size() == 3);
                        lightConfig.objects[el.first].color = {
                            cc::Color3B(std::stoi(lightColor[0]),
                                        std::stoi(lightColor[1]),
                                        std::stoi(lightColor[2])),
                            cc::Color3B(std::stoi(lightColor[0]),
                                        std::stoi(lightColor[1]),
                                        std::stoi(lightColor[2]))
                        };
                    }
                    
                    auto lightSize = std::vector<std::string>();
                    lib::split(info.at("size").asString(), lightSize, ",|", true);
                    
                    if (lightSize.size() == 4)
                    {
                        lightConfig.objects[el.first].size = {
                            cc::Size(std::stof(lightSize[0]), std::stof(lightSize[1])),
                            cc::Size(std::stof(lightSize[2]), std::stof(lightSize[3]))
                        };
                    }
                    else
                    {
                        assert(lightSize.size() == 2);
                        lightConfig.objects[el.first].size = {
                            cc::Size(std::stof(lightSize[0]), std::stof(lightSize[1])),
                            cc::Size(std::stof(lightSize[0]), std::stof(lightSize[1]))
                        };
                    }
                    
                    auto lightOpacity = std::vector<std::string>();
                    lib::split(info.at("opacity").asString(), lightOpacity, ",|", true);
                    
                    if (lightOpacity.size() == 2)
                    {
                        lightConfig.objects[el.first].opacity = {
                            std::stoi(lightOpacity[0]), std::stoi(lightOpacity[1])
                        };
                    }
                    else
                    {
                        assert(lightOpacity.size() == 1);
                        lightConfig.objects[el.first].opacity = {
                            std::stoi(lightOpacity[0]), std::stoi(lightOpacity[0])
                        };
                    }
                }
            }
            
            this->lightConfig = lightConfig;
        }
    }
    
    //ressources
    CCASSERT(rawData.find("res") != rawData.end(), "invalid map data");
    auto res = rawData.at("res").asValueMap();
    
    CCASSERT(res.find("ss") != res.end(), "invalid ss data");
    for(auto ss : res.at("ss").asValueVector())
    {
        this->spriteSheets.insert(ss.asString());
    }
    if (res.find("tex") != res.end())
    {
        for(auto tex : res.at("tex").asValueVector())
        {
            this->textures.insert(tex.asString());
        }
    }
    
    //design
    CCASSERT(rawData.find("design") != rawData.end(), "invalid map data");
    auto design = rawData.at("design").asValueMap();
    auto bgRawColor = std::vector<std::string>();
    lib::split(design.at("bg_color").asString(), bgRawColor, ",", true);
    this->bgColor = cc::Color3B(std::stoi(bgRawColor[0]),
                                std::stoi(bgRawColor[1]),
                                std::stoi(bgRawColor[2]));
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

std::pair<std::string, cc::Rect> FloorData::subExtractGateInfo(const cc::ValueMap& el)
{
    CCASSERT(el.find("tile") != el.end(), "invalid gate info");
    CCASSERT(el.find("bounds") != el.end(), "invalid gate info");
    
    auto tileName = el.at("tile").asString();
    auto tileBounds = std::vector<std::string>();
    lib::split(el.at("bounds").asString(), tileBounds, ",", true);

    cc::Rect gateBounds = cc::Rect(
        std::stof(tileBounds[0]),
        std::stof(tileBounds[1]),
        std::stof(tileBounds[2]),
        std::stof(tileBounds[3])
    );
    
    return {tileName, gateBounds};
}

FloorData::FloorData(const std::string& fileName) :
    random(Randgine::instance()->get(Randgine::MAP_DATA))
{
    this->floorMapping = nullptr;
    this->extractInfo(fileName);
}

const ModelVector FloorData::getModels(const std::string& category)
{
    if (!lib::hasKey(this->modelCategory, category))
        return ModelVector();
    return this->modelCategory[category];
}

FloorData* FloorData::generate(const std::string& filename)
{
    FloorData* map = new FloorData(filename);
    
    std::map<std::string, ModelVector>  modelList;
    
    for(auto modelCat : {"common", "empty"})
    {
        modelList[modelCat] = ModelVector(map->getModels(modelCat));
        std::shuffle(modelList[modelCat].begin(),
                     modelList[modelCat].end(),
                     map->random.getEngine());
    }

    //starter
    auto firstModel = FloorData::pickModel(modelList["empty"]);
    auto firstRoom = map->addRoom(firstModel, {
        .pos = map->floorMapping->getStartPosition(firstModel),
        .nbGates = map->startNbGates,
        .profile = RoomData::RoomType::STARTER
    });
    firstRoom->depth = 1;
    map->curIdxRoom = firstRoom->index;
    
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
        else if (currentDepth <= map->depthConfig.second - 1)
        {
            if (!endRoomPlaced)
            {
                targetProfile = RoomData::RoomType::FINISHER;
                endRoomPlaced = true;
                targetModel = "empty";
            }
            else
            {
                if (currentDepth < map->depthConfig.second - 1)
                    nbGates = {1, 3};
            }
        }
    
        for(int i = 0; i < modelList["common"].size(); i++)
        {
            auto model = pickModel(modelList["common"]);
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
    
    for(auto& pair : map->rooms)
        pair.second->init();
    
    return map;
}

RoomModel* FloorData::pickModel(ModelVector& modelList)
{
    auto model = modelList.front();
    modelList.erase(std::remove(modelList.begin(), modelList.end(), model),
                    modelList.end());
    modelList.push_back(model);
    return model;
}

void FloorData::computeDepth(RoomData *room)
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

FloorData::~FloorData()
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

RoomData* FloorData::addRoom(RoomModel* model, const RoomData::Config& config)
{
    static unsigned roomIndex = def::startGroupIndex;
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
    
    //handle special gates (room warps)
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

RoomData* FloorData::getRoomAt(unsigned int idx)
{
    CCASSERT(lib::hasKey(rooms, idx), "room is missing");
    return this->rooms[idx];
}

RoomData* FloorData::getCurrentRoom()
{
    CCASSERT(lib::hasKey(rooms, this->curIdxRoom), "room is missing");
    return this->rooms[curIdxRoom];
}