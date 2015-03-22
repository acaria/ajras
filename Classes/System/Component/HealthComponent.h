#pragma once
#include "Headers.h"

struct HealthComponent
{
    void set(int hp)
    {
        this->maxHp = hp;
        this->hp = maxHp;
    }

    //input
    int maxHp;
    
    int hp;
};