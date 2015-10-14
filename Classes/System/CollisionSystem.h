#pragma once

class RoomData;

#include "BaseTickSystem.h"
#include "ECSGroup.h"
#include "CoreLib.h"
#include "SweptAABB.h"
#include "PositionComponent.h"
#include "CollisionComponent.h"
#include "V2.h"
#include "IMapData.h"

class CollisionSystem : public BaseTickSystem
{
public:
    CollisionSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {}
    virtual ~CollisionSystem();
    
    void init(IMapData* mapData);
    
    virtual void tick(double dt) final;
    virtual void animate(double dt, double tp) final {}
    
private:
    struct gridCollisionInfo
    {
        cocos2d::Rect   intersectRect;
        lib::v2u        gridPos;
        cocos2d::Rect   gridRect;
    };
    
    void reset();
    
    lib::Box bounce(const PositionComponent &cpPos,
                    const CollisionComponent &cpCol,
                    const cocos2d::Rect& target);
    
    cc::Vec2 slide(const cc::Rect &src,
                   const cc::Rect &target);
    
    std::list<cocos2d::Rect> getRectGridCollisions(const cocos2d::Rect& rect,
                                                   CollisionCategory cat);
    bool checkRoomCollision(const cocos2d::Rect& rect,
                            CollisionCategory cat);
    lib::v2u getGridPosIntersect(float x, float y);
    lib::v2u getGridPosIntersect(const cocos2d::Vec2& v);
    
    //fields
    std::map<CollisionCategory, lib::DataGrid<bool>*> grids;
    lib::v2u blockSize;
    lib::v2u roomSize;
};