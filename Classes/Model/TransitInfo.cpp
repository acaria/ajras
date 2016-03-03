#include "TransitInfo.h"

cc::Vec2 TransitInfo::deployTeamUnit(const GateMap& gate,
    def::team::Formation formation, unsigned position)
{
    auto gateDir = gate.info.getDir();
    
    cc::Vec2 dir = {0,0};
    
    if (position == 0)
        dir = {0,10};
    else if (position == 1)
        dir = {-10,0};
    else if (position == 2)
        dir = {10,0};
    
    if (dir.isZero())
        return dir;
    
    if (gateDir == Dir::Left)
        dir.rotate({0,0}, CC_DEGREES_TO_RADIANS(-90));
    else if (gateDir == Dir::Right)
        dir.rotate({0,0}, CC_DEGREES_TO_RADIANS(90));
    else if (gateDir == Dir::Up)
        dir.rotate({0,0}, CC_DEGREES_TO_RADIANS(-180));
    
    return dir;
}
