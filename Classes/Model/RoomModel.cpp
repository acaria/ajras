#include "Headers.h"

RoomModel* RoomModel::create(const std::string &fileName)
{
    auto str = cocos2d::FileUtils::getInstance()->getStringFromFile(
                                                                    "maps/" + fileName + ".json");
    
    assert(str != "");
    jsonxx::Object o;
    assert(o.parse(str));
    
    auto result = new RoomModel({(unsigned)o.get<jsonxx::Number>("width"),
                                 (unsigned)o.get<jsonxx::Number>("height")},
                                {(unsigned)o.get<jsonxx::Number>("tilewidth"),
                                 (unsigned)o.get<jsonxx::Number>("tileheight")});
    
    result->name = fileName;
    
    std::map<unsigned, std::string>                         tileNames;
    std::map<unsigned, std::map<std::string, std::string>>  tileProperties;
    
    for (const auto &s : o.get<jsonxx::Array>("tilesets").values())
    {
        auto tileset = s->get<jsonxx::Object>();
        if (tileset.get<jsonxx::Object>("properties").has<jsonxx::String>("ss"))
        {
            //spritesheet mode
            auto ssName = tileset.get<jsonxx::Object>("properties").get<jsonxx::String>("ss");
            result->ss.push_back(ssName);
        }
        
        unsigned pindex = (unsigned) tileset.get<jsonxx::Number>("firstgid");
        
        for (const auto &t : tileset.get<jsonxx::Object>("tiles").kv_map())
        {
            unsigned index = stoi(t.first) + pindex;
            auto tile = t.second->get<jsonxx::Object>();
            std::string tmpPath = tile.get<jsonxx::String>("image");
            std::string tileName = tmpPath.substr(tmpPath.find_last_of("/\\") + 1);
            tileNames[index] = tileName;
        }
        
        if (tileset.has<jsonxx::Object>("tileproperties"))
        {
            for (const auto &t : tileset.get<jsonxx::Object>("tileproperties").kv_map())
            {
                unsigned index = stoi(t.first) + pindex;
                tileProperties[index] = std::map<std::string, std::string>();
                for(const auto &el : t.second->get<jsonxx::Object>().kv_map())
                    tileProperties[index][el.first] = el.second->get<jsonxx::String>();
            }
        }
    }
    
    for (const auto &l : o.get<jsonxx::Array>("layers").values())
    {
        auto layer = l->get<jsonxx::Object>();
        
        auto name = layer.get<jsonxx::String>("name");
        
        if (layer.has<jsonxx::Array>("data"))
        {
            //render mode
            unsigned gridIndex = 0;
            for(const auto &d : layer.get<jsonxx::Array>("data").values())
            {
                unsigned tileIndex = d->get<jsonxx::Number>();
                
                if (tileIndex == 0)
                {
                    gridIndex++;
                    continue;
                }
                
                if (tileNames.find(tileIndex) == tileNames.end())
                {
                    CCLOG("incorrect tileindex=%d", tileIndex);
                }
                else
                {
                    unsigned c = gridIndex % result->grid.width;
                    unsigned r = result->grid.height - 1 - gridIndex / result->grid.height;
                    
                    if (name == "background")
                        result->grid.get({c, r}).fields[BlockInfo::bgTileName] = tileNames[tileIndex];
                    else if (name == "foreground")
                        result->grid.get({c, r}).fields[BlockInfo::fgTileName] = tileNames[tileIndex];
                    else //filling properties
                    {
                        if (tileProperties.find(tileIndex) != tileProperties.end())
                        {
                            for(const auto &prop : tileProperties[tileIndex])
                            {
                                if (prop.first == "collision")
                                {
                                    result->grid.get({c, r}).fields[BlockInfo::collision] = prop.second;
                                }
                                else
                                {
                                    Log("invalid property name: %s", prop.first.c_str());
                                }
                            }
                        }
                    }
                }
                
                gridIndex++;
            }
        }
        else if (layer.has<jsonxx::Array>("objects"))
        {
            for(const auto &o : layer.get<jsonxx::Array>("objects").values())
            {
                auto object = o->get<jsonxx::Object>();
                
                unsigned tileIndex = object.get<jsonxx::Number>("gid");
                if (tileNames.find(tileIndex) == tileNames.end())
                {
                    CCLOG("incorrect tileindex=%d", tileIndex);
                }
                
                result->objs.push_back({
                    .pos = {
                        (float)object.get<jsonxx::Number>("x"),
                        result->totalSize.y - (float)object.get<jsonxx::Number>("y")
                    },
                    .profileName = tileProperties[object.get<jsonxx::Number>("gid")]["profile"]
                });
            }
        }
    }
    
    processing(*result);
    genShape(*result);

    return result;
}

GateInfo RoomModel::extractGate(RoomModel& mapData, const std::string& collisionType,
                                unsigned i, unsigned j)
{
    GateInfo gate {
        .rect = mapData.getRectCoord({i,j}),
        .type = GateInfo::Unknowm
    };
    
    int maxK = mapData.grid.width;
    int maxL = mapData.grid.height;
    bool yAxis = false;
    for(unsigned l = j; l < maxL; l++)
        for(unsigned k = i; k < maxK; k++)
        {
            auto &block = mapData.grid.get({k, l});
            if (block.fields.find(BlockInfo::collision) != block.fields.end() &&
                block.fields[BlockInfo::collision] == collisionType)
            {
                gate.rect = mapData.getRectCoord({k,l}).unionWithRect(gate.rect);
                if (gate.type == GateInfo::Unknowm)
                    gate.type = gessGateType({k,l}, mapData);
                block.fields[BlockInfo::collision] = "walkable";
            }
            else
            {
                if (yAxis)
                    maxL = l;
                else
                {
                    yAxis = true;
                    maxK = k;
                }
            }
        }
    
    CCASSERT(gate.type != GateInfo::Unknowm, "gate extraction error");
    return gate;
}

void RoomModel::processing(RoomModel& mapData)
{
    unsigned gateIndex=1;
    for(unsigned j = 0; j < mapData.grid.height; j++)
    for(unsigned i = 0; i < mapData.grid.width; i++)
    {
        auto &block = mapData.grid.get({i,j});
        if (block.fields.find(BlockInfo::collision) != block.fields.end())
        {
            if (block.fields[BlockInfo::collision] == "crossing")
            {
                mapData.gates[gateIndex++] = extractGate(mapData, "crossing", i, j);
            }
            else if (block.fields[BlockInfo::collision] == "warping")
            {
                mapData.warps.push_back(extractGate(mapData, "warping", i, j));
            }
        }
    }
}

GateInfo::GateType RoomModel::gessGateType(lib::v2u pos, const RoomModel& mapData)
{
    if (pos.x == 0)
        return GateInfo::Left;
    if (pos.y == 0)
        return GateInfo::Down;
    if (pos.x == mapData.grid.width - 1)
        return GateInfo::Right;
    if (pos.y == mapData.grid.height - 1)
        return GateInfo::Up;
    return GateInfo::Unknowm;
}

cocos2d::Rect RoomModel::getRectCoord(lib::v2u pos)
{
    return { (float)pos.x * tileSize.x, (float)pos.y * tileSize.y,
             (float)tileSize.x, (float)tileSize.y };
}

cocos2d::Vec2 RoomModel::getPosCoord(lib::v2u pos)
{
    return { (float)pos.x * tileSize.x, (float)pos.y * tileSize.y };
}

lib::v2u RoomModel::getGridPos(cocos2d::Vec2 coord)
{
    return {(unsigned)coord.x / tileSize.x,
            (unsigned)coord.y / tileSize.y
    };
}

int RoomModel::getZOrder(const cocos2d::Vec2& pos)
{
    return (totalSize.x - pos.x + ((totalSize.y - pos.y) * totalSize.y));
}

void RoomModel::genShape(RoomModel& mapData)
{
    mapData.shape.walls.push_back(cocos2d::Rect(0, 0,
        mapData.grid.width, mapData.grid.height));
    
    for(auto pair : mapData.gates)
    {
        auto gate = pair.second;
        lib::v2i p;
        
        if (gate.type == GateInfo::Left)
        {
            p = {(int)(gate.rect.getMinX() / mapData.tileSize.x - 1),
                (int)(gate.rect.origin.y / mapData.tileSize.y)};
        }
        else if (gate.type == GateInfo::Right)
        {
            p = {(int)(gate.rect.getMaxX() / mapData.tileSize.x),
                (int)(gate.rect.origin.y / mapData.tileSize.y)};
        }
        else if (gate.type == GateInfo::Up)
        {
            p = {(int)(gate.rect.origin.x / mapData.tileSize.x),
                (int)(gate.rect.getMaxY() / mapData.tileSize.y)};
        }
        else if (gate.type == GateInfo::Down)
        {
            p = {(int)(gate.rect.origin.x / mapData.tileSize.x),
                (int)(gate.rect.getMinY() / mapData.tileSize.y - 1)};
        }
        else
        {
            Log("invalid gate");
        }
        mapData.shape.gates[pair.first] = p;
    }
}
