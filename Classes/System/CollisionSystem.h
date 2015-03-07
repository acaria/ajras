#pragma once
#include "Headers.h"

class CollisionSystem : public BaseTickSystem
{
public:
    CollisionSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {}
    virtual ~CollisionSystem();
    
    void init(RoomData* data);
    
    virtual void tick(double dt) final;
    virtual void animate(double dt, double tp) final;
    
private:
    struct gridCollisionInfo
    {
        cocos2d::Rect   intersectRect;
        lib::v2u        gridPos;
        cocos2d::Rect   gridRect;
    };

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