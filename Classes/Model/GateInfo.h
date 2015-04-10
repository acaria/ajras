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
    
    cc::Point getSrcPos()
    {
        switch(type)
        {
            case Left: return {rect.getMinX(), rect.getMidY()};
            case Right: return {rect.getMaxX(), rect.getMidY()};
            case Up: return {rect.getMidX(), rect.getMaxY()};
            case Down: return {rect.getMidX(), rect.getMinY()};
            default: return {rect.getMidX(), rect.getMidY()};
        }
    }
    
    cc::Point getDestPos()
    {
        switch(type)
        {
            case Left: return {rect.getMaxX(), rect.getMidY()};
            case Right: return {rect.getMinX(), rect.getMidY()};
            case Up: return {rect.getMidX(), rect.getMinY()};
            case Down: return {rect.getMidX(), rect.getMaxY()};
            default: return {rect.getMidX(), rect.getMidY()};
        }
    }

    
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