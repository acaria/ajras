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
    ~CollisionInfo() {}
    void process();
    void init(IMapData* mapData);
    
    std::list<cc::Rect> getAgentBounds(const std::set<unsigned>& eids,
                                       def::collision::Cat cat);
    
    bool checkCollisionRect(const cocos2d::Rect& rect, def::collision::Cat cat);
    bool checkCollisionRay(const cc::Point& origin, const cc::Point& dest,
                           unsigned eid, unsigned oid, ColCat cat);

    std::vector<cc::Rect> getNearEmptyBlocks(const lib::v2u& coord, unsigned maxDist, ColCat cat);
    std::vector<cc::Rect> getNearEmptyBlocks(const cc::Point& pos, unsigned maxDist, ColCat cat);
    std::vector<lib::v2u> getAllStaticFreeCoords(ColCat cat);
    
    cc::Point getFormationPosition(def::team::Formation formation,
                                   unsigned position,
                                   const std::list<lib::v2u>& tail);
    
    //events
    lib::Subject<void(unsigned, cc::Vec2 diff)>             onDecorCollision;
    lib::Subject<void(unsigned, unsigned, cc::Vec2 diff)>   onAgentCollision;
    lib::Subject<void(unsigned, cc::Vec2 diff)>             onFakeNodeCollision;
    
    //dynamic fields
    std::map<unsigned, Agent>       agents;
    std::map<cc::Node*, cc::Rect>   fakeNodeAgents;
    std::map<unsigned, cc::Rect>    fakeRectAgents;
    
private:
    cc::Point getCollisionDiff(const cc::Rect& destBounds, const cc::Rect& lastBounds, ColCat cat);
    
    void reset();
    void prepare();
    void processAgent(const Agent& agent);
    void processFakeNode(cc::Node* node, const cc::Rect& shape);
    void processFakeRect(const cc::Rect& bounds);
    
    std::list<cocos2d::Rect>    getRectGridCollisions(const cocos2d::Rect& rect, ColCat cat);
    
    //fields
    std::map<ColCat, lib::DataGrid<bool>>   grids;
    std::map<ColCat, std::vector<lib::v2u>> freeStaticCoords;
    
    IMapData* data;
};