#pragma once
#include "Headers.h"

struct RoomShape
{
    lib::v2i                     position;
    std::list<cocos2d::Rect>     walls;
    std::map<unsigned, lib::v2i> gates;
};