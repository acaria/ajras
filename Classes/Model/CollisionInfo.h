#pragma once

#include "Defines.h"
#include "CoreLib.h"
#include "IMapData.h"

class CollisionInfo
{
public:
    ~CollisionInfo();
    void reset();
    void process();
    void init(IMapData* mapData);
    
    std::list<cocos2d::Rect> getRectGridCollisions(const cocos2d::Rect& rect,
                                                   CollisionCategory cat);
    bool checkRoomCollision(const cocos2d::Rect& rect,
                            CollisionCategory cat);
    
private:
    std::map<CollisionCategory, lib::DataGrid<bool>*> grids;
    IMapData* data;
};