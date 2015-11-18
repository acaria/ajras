#pragma once

#include "Defines.h"
#include "CoreLib.h"
#include "DataGrid.h"
#include "Random.h"
#include "Dir.h"

class IMapData;
class DatGrid;
class NavigationInfo;

class CollisionInfo
{
    friend NavigationInfo;

public:
    ~CollisionInfo();
    void reset();
    void process();
    void init(IMapData* mapData);
    
    cc::Point getCollisionPos(const cc::Rect& destBounds,
                              const cc::Rect& lastBounds,
                              CollisionCategory cat);

    bool checkCollisionRect(const cocos2d::Rect& rect,
                            CollisionCategory cat);

    bool checkCollisionRay(const cc::Point& origin,
                           const cc::Point& dest,
                           CollisionCategory cat);

    std::vector<cc::Rect> getNearEmptyBlocks(const lib::v2u& coord,
                                             unsigned maxDist,
                                             CollisionCategory cat);
    std::vector<cc::Rect> getNearEmptyBlocks(const cc::Point& pos,
                                             unsigned maxDist,
                                             CollisionCategory cat);
    
private:
    //internal
    struct SweepInfo
    {
        float       count;
        cc::Point   dir;
        cc::Point   tx;
    };
    SweepInfo InterpolateDir(const cc::Point& dir, const cc::Rect& bounds);
    
    std::list<cocos2d::Rect> getRectGridCollisions(const cocos2d::Rect& rect,
                                                   CollisionCategory cat);
    bool needMerge(const cc::Rect& r1, const cc::Rect& r2);
    std::list<cc::Rect> mergeRectGrids(std::list<cc::Rect> src);

    //fields
    std::map<CollisionCategory, lib::DataGrid<bool>*> grids;
    IMapData* data;
};