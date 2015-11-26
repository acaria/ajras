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
    using ColCat = def::collision::Cat;
    using Agent = def::collision::Agent;
    
    friend NavigationInfo;

public:
    ~CollisionInfo();
    void process();
    void init(IMapData* mapData);
    
    bool checkCollisionRect(const cocos2d::Rect& rect, def::collision::Cat cat);
    bool checkCollisionRay(const cc::Point& origin, const cc::Point& dest, ColCat cat);

    std::vector<cc::Rect> getNearEmptyBlocks(const lib::v2u& coord, unsigned maxDist, ColCat cat);
    std::vector<cc::Rect> getNearEmptyBlocks(const cc::Point& pos, unsigned maxDist, ColCat cat);
    
    //events
    lib::Subject<void(unsigned, cc::Vec2 diff)>             onDecorCollision;
    lib::Subject<void(unsigned, unsigned, cc::Vec2 diff)>   onAgentCollision;
    
    //dynamic fields
    std::map<unsigned, Agent> agents;
    
private:
    cc::Point getCollisionDiff(const cc::Rect& destBounds, const cc::Rect& lastBounds, ColCat cat);
    
    void reset();
    void prepare();
    void processAgent(const Agent& agent);
    
    std::list<cocos2d::Rect>    getRectGridCollisions(const cocos2d::Rect& rect, ColCat cat);
    
    //fields
    std::map<ColCat, lib::DataGrid<bool>*>  grids;
    
    IMapData* data;
};