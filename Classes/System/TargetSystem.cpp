#include "TargetSystem.h"
#include "Components.h"
#include "SysHelper.h"

void TargetSystem::tick(double dt)
{
    for(auto eid : ecs.join<cp::Target, cp::Position, cp::Physics>())
    {
        unsigned eid2 = ecs::get<cp::Target>(eid);
        if (eid2 == 0 || !ecs::has<cp::Position, cp::Physics>(eid2))
            continue;
        
        cc::Rect r1 = SysHelper::getBounds(eid);
        cc::Rect r2 = SysHelper::getBounds(eid2);
        
        cc::Point pdir = cc::Point(r2.getMidX(), r2.getMidY()) -
        cc::Point(r1.getMidX(), r1.getMidY());
        
        Dir odir = Dir::None;
        
        if (fabs(pdir.x) > fabs(pdir.y))
            odir = pdir.x < 0 ? Dir::Left : Dir::Right;
        else
            odir = pdir.y < 0 ? Dir::Down : Dir::Up;
        
        ecs::get<cp::Position>(eid).dir = odir;
    }
}

