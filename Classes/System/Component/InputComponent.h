#pragma once
#include "Dir.h"
#include "CoreLib.h"
#include "Defines.h"

struct InputComponent
{
    //output
    cc::Vec2                direction;
    lib::Nullable<cc::Vec2> goTo;
    
    //mode
    bool enabled = true;
};