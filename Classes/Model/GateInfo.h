#pragma once

#include "Dir.h"

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

    cc::Rect   rect;
    GateType   type;
    
    static GateType typeFromStr(std::string str)
    {
        if (str == "left")
            return GateType::Left;
        if (str == "right")
            return GateType::Right;
        if (str == "up")
            return GateType::Up;
        if (str == "down")
            return GateType::Down;
        if (str == "upstair")
            return GateType::Upstair;
        if (str == "downStair")
            return GateType::DownStair;
        return GateType::Unknowm;
    }
    
    Dir getDir() const
    {
        switch(type)
        {
            case Left: return Dir::Right;
            case Right: return Dir::Left;
            case Up: return Dir::Down;
            case Down: return Dir::Up;
            default: return Dir::None;
        }
    }
    
    cc::Point getSrcPos() const
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
    
    cc::Point getDestPos() const
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