#include "Headers.h"

void TargetSystem::tick(double dt)
{
    for(auto eid : ecs.join<cp::Target, cp::Orientation, cp::Position>())
    {
        unsigned eid2 = ecs::get<cp::Target>(eid);
        if (eid2 == 0 || !ecs::has<cp::Position>(eid2))
            continue;
        
        Point pos1 = ecs::get<cp::Position>(eid).pos;
        Point pos2 = ecs::get<cp::Position>(eid2).pos;
        
        Point pdir = pos2 - pos1;
        
        Dir odir = Dir::kNone;
        
        if (abs(pdir.x) > abs(pdir.y))
            odir = pdir.x < 0 ? Dir::kLeft : Dir::kRight;
        else
            odir = pdir.y < 0 ? Dir::kDown : Dir::kUp;
        
        ecs::get<cp::Orientation>(eid).curDir = odir;
    }
}

