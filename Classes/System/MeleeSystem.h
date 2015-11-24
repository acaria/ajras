#pragma once

#include "Event.h"

namespace lib
{
    class EcsGroup;
}
class Dir;
class GameScene;
class MeleeComponent;
class IMapData;

#include "BaseSystem.h"
#include "CollisionInfo.h"

class MeleeSystem : public BaseSystem
{
public:
    virtual ~MeleeSystem() {}
    
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
    
private:
    void processDirMelee(unsigned eid, unsigned oid, Dir atkDir);
    void processTouchMelee(unsigned eid, unsigned oid);
    Dir getAtkDir(unsigned eid, const MeleeComponent& cpMelee);
    cocos2d::Rect getAtkRectFromDir(const cocos2d::Rect& bounds,
                                    cc::Size range, const Dir& dir);
    
    GameScene* gView;
};