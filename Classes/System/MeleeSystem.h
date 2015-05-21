#pragma once

#include "Event.h"

namespace lib
{
    class EcsGroup;
}
class Dir;
class GameScene;

#include "BaseTickSystem.h"

class MeleeSystem : public BaseTickSystem
{
public:
    MeleeSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {}
    
    virtual ~MeleeSystem() {}
    
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
    
    lib::Subject<void(unsigned eid, int health)> onHealthChanged;
    
private:
    cocos2d::Rect getAtkRectFromDir(const cocos2d::Rect& bounds,
                                    unsigned range,
                                    const Dir& dir);
    bool detectStriking(unsigned eid);
    
    GameScene* gView;
};