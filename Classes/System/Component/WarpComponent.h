#pragma once
#include "Headers.h"

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