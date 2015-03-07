#pragma once
#include "Headers.h"

struct GateComponent
{
    void set(unsigned destRoomIdx, unsigned destGateIdx, GateInfo info)
    {
        this->destRoomIdx = destRoomIdx;
        this->destGateIdx = destGateIdx;
        this->info = info;
    }
    
    unsigned destRoomIdx;
    unsigned destGateIdx;
    GateInfo info;
};