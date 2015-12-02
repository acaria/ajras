#pragma once

#include "Event.h"

class MeleeComponent;

#include "BaseSystem.h"
#include "CollisionInfo.h"

class MeleeSystem : public BaseSystem
{
public:
    virtual ~MeleeSystem() {}
    
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
    
private:

    struct blinkAnim
    {
        static constexpr float duration = 0.3f;
        static constexpr int   count = 3;
        static constexpr int   tag = 99;
    };

    void processDirMelee(unsigned eid, unsigned oid, Dir atkDir);
    void processTouchMelee(unsigned eid, unsigned oid);
    Dir getAtkDir(unsigned eid, const MeleeComponent& cpMelee);
    cocos2d::Rect getAtkRectFromDir(const cocos2d::Rect& bounds,
                                    cc::Size range, const Dir& dir);
    
    void setEntityAvailability(unsigned eid, bool enabled);
};