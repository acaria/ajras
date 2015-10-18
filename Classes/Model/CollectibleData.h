#pragma once

#include "Defines.h"
#include "CoreLib.h"

struct CollectibleData
{
    //ID
    std::string key;
    
    //data
    unsigned    currencyValue;
    unsigned    stackability;
    
    //view
    std::string spriteFrameName;
    bool        spriteRotate;
};

