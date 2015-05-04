#include "RoomModel.h"
#include "Jsonxx.h"
#include "Misc.h"

RoomModel* RoomModel::create(const std::string &fileName)
{
    auto str = cc::FileUtils::getInstance()->getStringFromFile(
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
    
    //todo: define walls for non rectangular rooms
    result->walls.push_back({
        0,
        0,
        (float)result->totalSize.x,
        (float)result->totalSize.y});
    
    //processing cross areas
    for(unsigned j = 0; j < result->grid.height; j++)
    for(unsigned i = 0; i < result->grid.width; i++)
    {
        auto &block = result->grid.get({i,j});
        if (block.fields.find(BlockInfo::collision) != block.fields.end())
        {
            if (block.fields[BlockInfo::collision] == "crossing")
                result->crossAreas.push_back(extractCrossArea(*result, {i, j}));
        }
    }
    
    //genShape(*result);

    return result;
}

/*void RoomModel::extractGateringIntroInfo(unsigned gateIndex, cc::Rect colRect,
                                         cc::Point& srcPos, cc::Point& destPos)
{
    cocos2d::Vec2 movePos;
    auto gateInfo = this->gates[gateIndex];
    
    switch(gateInfo.type)
    {
        case GateInfo::Left:
            srcPos = {gateInfo.rect.getMinX() - colRect.getMinX(),
                gateInfo.rect.getMinY() + (gateInfo.rect.size.height - colRect.size.height) / 2 - colRect.getMinY()};
            movePos = {gateInfo.rect.size.width,0};
            break;
        case GateInfo::Right:
            srcPos = {gateInfo.rect.getMaxX() - colRect.size.width - colRect.getMinX(),
                gateInfo.rect.getMinY() + (gateInfo.rect.size.height - colRect.size.height) / 2 - colRect.getMinY()};
            movePos = {-gateInfo.rect.size.width,0};
            break;
        case GateInfo::Up:
            srcPos = {gateInfo.rect.getMinX() + (gateInfo.rect.size.width - colRect.size.width) / 2 - colRect.getMinX(),
                gateInfo.rect.getMaxY() - colRect.size.height - colRect.getMinY()};
            movePos = {0,-gateInfo.rect.size.height};
            break;
        case GateInfo::Down:
            srcPos = {gateInfo.rect.getMinX() + (gateInfo.rect.size.width - colRect.size.width) / 2 - colRect.getMinX(),
                gateInfo.rect.getMinY() - colRect.getMinY()};
            movePos = {0,gateInfo.rect.size.height};
            break;
        default:
            Log("2: unsupported gate type detected");
            break;
    }
    
    destPos = srcPos + movePos;
}*/


GateInfo RoomModel::extractCrossArea(RoomModel& roomModel, lib::v2u coord)
{
    GateInfo gate {
        .rect = roomModel.getRectCoord({coord.x,coord.y}),
        .type = GateInfo::Unknowm
    };
    
    int maxK = roomModel.grid.width;
    int maxL = roomModel.grid.height;
    bool yAxis = false;
    for(unsigned l = coord.y; l < maxL; l++)
        for(unsigned k = coord.x; k < maxK; k++)
        {
            auto &block = roomModel.grid.get({k, l});
            if (block.fields.find(BlockInfo::collision) != block.fields.end() &&
                block.fields[BlockInfo::collision] == "crossing")
            {
                gate.rect = roomModel.getRectCoord({k,l}).unionWithRect(gate.rect);
                if (gate.type == GateInfo::Unknowm)
                    gate.type = gessGateType({k,l}, roomModel);
                block.fields.erase(BlockInfo::collision);// = "walkable";
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

/*void RoomModel::processing(RoomModel& roomModel)
{
    for(unsigned j = 0; j < roomModel.grid.height; j++)
    for(unsigned i = 0; i < roomModel.grid.width; i++)
    {
        auto &block = roomModel.grid.get({i,j});
        if (block.fields.find(BlockInfo::collision) != block.fields.end())
        {
            if (block.fields[BlockInfo::collision] == "crossing")
                roomModel.crossAreas.push_back(extractCrossArea(roomModel, i, j));
        }
    }
}*/

GateInfo::GateType RoomModel::gessGateType(lib::v2u pos, const RoomModel& roomModel)
{
    if (pos.x == 0)
        return GateInfo::Left;
    if (pos.y == 0)
        return GateInfo::Down;
    if (pos.x == roomModel.grid.width - 1)
        return GateInfo::Right;
    if (pos.y == roomModel.grid.height - 1)
        return GateInfo::Up;
    return GateInfo::Unknowm;
}

cocos2d::Rect RoomModel::getRectCoord(lib::v2u pos)
{
    return { (float)pos.x * tileSize.x, (float)pos.y * tileSize.y,
             (float)tileSize.x, (float)tileSize.y };
}

cocos2d::Vec2 RoomModel::getPosFromCoord(lib::v2u coord)
{
    return {(float)coord.x * tileSize.x, (float)coord.y * tileSize.y};
}

lib::v2u RoomModel::getCoordFromPos(cocos2d::Vec2 pos)
{
    return {(unsigned)pos.x / tileSize.x, (unsigned)pos.y / tileSize.y};
}

int RoomModel::getZOrder(const cocos2d::Vec2& pos)
{
    return (totalSize.x - pos.x + ((totalSize.y - pos.y) * totalSize.y));
}