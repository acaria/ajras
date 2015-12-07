#include "TmxDataModel.h"
#include "CoreLib.h"

TmxDataModel::TmxDataModel(const std::string &fileName) : grid(0,0)
{
    auto str = cc::FileUtils::getInstance()->getStringFromFile(
        "maps/" + fileName + ".json");
    
    assert(str != "");
    jsonxx::Object o;
    assert(o.parse(str));
    
    this->grid = {(unsigned)o.get<jsonxx::Number>("width"),
        (unsigned)o.get<jsonxx::Number>("height")};
    
    this->tileSize = {(unsigned)o.get<jsonxx::Number>("tilewidth"),
                      (unsigned)o.get<jsonxx::Number>("tileheight")};
    
    this->totalSize = {grid.width * tileSize.x, grid.height * tileSize.y};
    
    this->name = fileName;
    
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
                    Log("incorrect tileindex=%d", tileIndex);
                }
                else
                {
                    unsigned c = gridIndex % this->grid.width;
                    unsigned r = this->grid.height - 1 - gridIndex / this->grid.width;
                    
                    if (name == "background")
                        this->grid.get({c, r}).fields[BlockInfo::bgTileName] = tileNames[tileIndex];
                    else if (name == "foreground")
                        this->grid.get({c, r}).fields[BlockInfo::fgTileName] = tileNames[tileIndex];
                    
                    if (tileProperties.find(tileIndex) != tileProperties.end())
                    {
                        for(const auto &prop : tileProperties[tileIndex])
                        {
                            switch(lib::hash(prop.first))
                            {
                                case lib::hash("collision"):
                                    this->grid[{c, r}].fields[BlockInfo::collision] = prop.second;
                                    break;
                                case lib::hash("sleep_zone"):
                                    this->grid[{c, r}].fields[BlockInfo::sleepZone] = prop.second;
                                    break;
                                case lib::hash("sleep_cat"):
                                    this->grid[{c, r}].fields[BlockInfo::sleepCat] = prop.second;
                                    break;
                                default:
                                    Log("invalid property name: %s", prop.first.c_str());
                                    break;
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
                    Log("incorrect tileindex=%d", tileIndex);
                }
                
                ObjectInfo res = {
                    .name = object.get<jsonxx::String>("name"),
                    .type = object.get<jsonxx::String>("type"),
                    .pos = {
                        (float)object.get<jsonxx::Number>("x"),
                        this->totalSize.y - (float)object.get<jsonxx::Number>("y")
                    },
                    .size = {
                        (float)object.get<jsonxx::Number>("width"),
                        (float)object.get<jsonxx::Number>("height")
                    },
                    .properties = {}
                };
                
                for(auto cpl : tileProperties[object.get<jsonxx::Number>("gid")])
                {
                    if (cpl.first == "type" && res.type == "") //type override
                        res.type = cpl.second;
                    res.properties.insert(cpl);
                }
                
                if (object.has<jsonxx::Object>("properties"))
                {
                    for(const auto &el : object.get<jsonxx::Object>("properties").kv_map())
                    {
                        res.properties[el.first] = el.second->get<jsonxx::String>();
                    }
                }
                
                this->objs.push_back(res);
            }
        }
    }
}

cocos2d::Rect TmxDataModel::getRectCoord(const lib::v2u& pos)
{
    return {(float)pos.x * tileSize.x, (float)pos.y * tileSize.y,
            (float)tileSize.x, (float)tileSize.y};
}

cocos2d::Vec2 TmxDataModel::getPosFromCoord(const lib::v2u& coord)
{
    return {(float)(coord.x * tileSize.x), (float)(coord.y * tileSize.y)};
}

lib::v2u TmxDataModel::getCoordFromPos(const cocos2d::Vec2& pos)
{
    return {(unsigned)pos.x / tileSize.x, (unsigned)pos.y / tileSize.y};
}