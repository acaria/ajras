#pragma once
#include "Headers.h"

struct GateInfo
{
    enum GateType
    {
        Unknowm,
        Left,
        Right,
        Up,
        Down,
        Upstair,
        DownStair
    };

    cocos2d::Rect   rect;
    GateType        type;
    
    GateType getOpposite()
    {
        if (type == Left)
            return Right;
        if (type == Right)
            return Left;
        if (type == Up)
            return Down;
        if (type == Down)
            return Up;
        if (type == Upstair)
            return DownStair;
        if (type == DownStair)
            return Upstair;
        return Unknowm;
    }
};