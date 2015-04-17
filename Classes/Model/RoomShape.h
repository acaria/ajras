#pragma once
#include "Misc.h"

struct RoomShape
{
    lib::v2i                        position;
    std::list<cc::Rect>             walls;
    std::list<cc::Rect>             warps;
    std::map<unsigned, lib::v2i>    gates;
};