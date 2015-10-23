#pragma once

#include "Defines.h"
#include "CoreLib.h"
#include "IMapData.h"
#include "Random.h"

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
    std::vector<cc::Rect> getNearEmptyBlocks(const lib::v2u& coord,
                                          unsigned maxDist,
                                          CollisionCategory cat);
    std::vector<cc::Rect> getNearEmptyBlocks(const cc::Point& pos,
                                           unsigned maxDist,
                                           CollisionCategory cat);
    
private:
    //internal
    bool needMerge(const cc::Rect& r1, const cc::Rect& r2);
    std::list<cc::Rect> mergeRectGrids(std::list<cc::Rect> src);

    //fields
    std::map<CollisionCategory, lib::DataGrid<bool>*> grids;
    IMapData* data;
};