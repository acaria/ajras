#pragma once
#include "GateInfo.h"

struct WarpComponent
{
    void set(GateInfo info, std::function<void()> onWarp)
    {
        this->onWarp = onWarp;
        this->info = info;
    }
    
    std::function<void()> onWarp;
    GateInfo info;
};