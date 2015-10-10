#pragma once

#include "Jsonxx.h"
#include "DataGrid.h"
#include "BlockInfo.h"
#include "GateInfo.h"
#include "V2.h"
#include "ObjectInfo.h"
#include "AIComponent.h"

class TmxDataModel
{
public:
    TmxDataModel(const std::string &fileName);
    virtual ~TmxDataModel() {}

    std::vector<ObjectInfo>      objs;
    lib::DataGrid<BlockInfo>     grid;
    
    lib::v2u        tileSize; //sizeof a tile case (in pxl)
    lib::v2u        totalSize; //sizeof the room (in pxl)
    
    std::string     name;
    
    cocos2d::Rect getRectCoord(lib::v2u pos);
    cocos2d::Vec2 getPosFromCoord(lib::v2u coord);
    lib::v2u getCoordFromPos(cocos2d::Vec2 pos);
    int getZOrder(const cocos2d::Vec2& pos);
    
protected:
    jsonxx::Object o;
};