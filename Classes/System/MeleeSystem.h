#pragma once

#include "Event.h"

namespace lib
{
    class EcsGroup;
}
class Dir;
class GameScene;
class MeleeComponent;

#include "BaseTickSystem.h"
#include "CollisionInfo.h"

class MeleeSystem : public BaseTickSystem
{
public:
    MeleeSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {}
    virtual ~MeleeSystem() {}
    
    void init(CollisionInfo* collision);
    
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
    
    lib::Subject<void(unsigned eid, int health)> onHealthChanged;
    
private:
    void processDirMelee(unsigned eid, unsigned oid, Dir atkDir);
    void processTouchMelee(unsigned eid, unsigned oid);
    Dir getAtkDir(unsigned eid, const MeleeComponent& cpMelee);
    cocos2d::Rect getAtkRectFromDir(const cocos2d::Rect& bounds,
                                    cc::Size range, const Dir& dir);
    
    GameScene* gView;
    CollisionInfo* collision;
};