#include "Headers.h"

void TargetSystem::tick(double dt)
{
    for(auto eid : ecs.join<cp::Target, cp::Orientation, cp::Position, cp::Collision>())
    {
        unsigned eid2 = ecs::get<cp::Target>(eid);
        if (eid2 == 0 || !ecs::has<cp::Position, cp::Collision>(eid2))
            continue;
        
        Rect r1 = SysHelper::getBounds(eid);
        Rect r2 = SysHelper::getBounds(eid2);
        
        Point pdir = Point(r2.getMidX(), r2.getMidY()) -
                     Point(r1.getMidX(), r1.getMidY());
        
        Dir odir = Dir::kNone;
        
        if (abs(pdir.x) > abs(pdir.y))
            odir = pdir.x < 0 ? Dir::kLeft : Dir::kRight;
        else
            odir = pdir.y < 0 ? Dir::kDown : Dir::kUp;
        
        ecs::get<cp::Orientation>(eid).curDir = odir;
    }
}

