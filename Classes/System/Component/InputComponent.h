#pragma once
#include "Dir.h"
#include "CoreLib.h"
#include "Defines.h"

struct InputComponent
{
    //output
    cc::Vec2                direction;
    lib::Nullable<cc::Vec2> goTo;
    
    bool enabled = true;
    
#if ECSYSTEM_DEBUG
    std::list<cc::Point> wayPoints;
#endif
};