#pragma once

#include "Defines.h"

struct ISleepZones
{
    virtual ~ISleepZones() {}
    virtual def::SleepZone* getSleepZone(const std::string& cat, const cc::Point& pos) = 0;
    virtual void freeSleepZone(const std::string& cat, const cc::Point &pos) = 0;
};